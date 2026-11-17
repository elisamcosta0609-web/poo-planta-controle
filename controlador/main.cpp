#include "aquisicao.hpp"
#include "include/estado.hpp"
#include "include/politica_controle.hpp"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

bool gravar_json(const std::string& caminho, const json& dados) {
    std::string temporario = caminho + ".tmp";

    {
        std::ofstream arquivo(temporario);

        if (!arquivo.is_open()) {
            return false;
        }

        arquivo << dados.dump(2) << '\n';

        if (!arquivo) {
            arquivo.close();
            std::filesystem::remove(temporario);
            return false;
        }
    }

    try {
        std::filesystem::rename(temporario, caminho);
        return true;
    } catch (...) {
        std::filesystem::remove(temporario);
        return false;
    }
}

int main(int argc, char* argv[]) {
    if (argc == 2 && std::string(argv[1]) == "--ajuda") {
        std::cout << "Uso: controlador --dados DIRETORIO --uma-vez\n";
        return 0;
    }

    if (argc != 4 ||
        std::string(argv[1]) != "--dados" ||
        std::string(argv[3]) != "--uma-vez") {
        std::cerr << "Argumentos invalidos.\n";
        return 1;
    }

    std::filesystem::path diretorio = argv[2];

    std::string caminho_planta =
        (diretorio / "planta.json").string();

    std::string caminho_telemetria =
        (diretorio / "telemetria.json").string();

    std::string caminho_comando =
        (diretorio / "comando_planta.json").string();

    std::string caminho_estado =
        (diretorio / "controlador_estado.json").string();

    Aquisicao aquisicao;
    Estado estado;
    PoliticaControle politica;

    json dados;
    std::string erro;

    if (!aquisicao.carregar(caminho_planta, dados, erro)) {
        std::cerr << erro << '\n';
        return 1;
    }

    std::string sessao = dados.at("sessao").get<std::string>();
    int ciclo = dados.at("ciclo").get<int>();
    int confirmado = dados.at("ultimo_comando").get<int>();

    bool estado_carregado = estado.carregar(caminho_estado);

    bool nova_sessao =
        !estado_carregado || estado.sessao != sessao;

    if (nova_sessao) {
        estado = Estado();
        estado.sessao = sessao;
        estado.ultimo_confirmado = confirmado;
    } else {
        estado.confirmar_comando(confirmado);

        estado.ultimo_confirmado =
            std::max(estado.ultimo_confirmado, confirmado);

        if (!estado.salvar(caminho_estado)) {
            std::cerr << "Erro ao salvar a confirmacao do comando.\n";
            return 1;
        }
    }

    bool novo_ciclo = nova_sessao || estado.ciclo != ciclo;

    if (estado.tem_pendente) {
        if (novo_ciclo) {
            estado.atualizar_ciclo(sessao, ciclo);
        }

        if (!estado.salvar(caminho_estado)) {
            std::cerr << "Erro ao salvar o estado do controlador.\n";
            return 1;
        }
    } else if (novo_ciclo) {
        double nivel = 0.0;
        double temperatura = 0.0;

        bool encontrou_nivel = false;
        bool encontrou_temperatura = false;

        for (const auto& leitura : dados["leituras"]) {
            std::string tipo = leitura.at("tipo").get<std::string>();
            std::string status = leitura.at("status").get<std::string>();

            if (status != "operando" || leitura.at("valor").is_null()) {
                if (tipo == "nivel") {
                    encontrou_nivel = false;
                } else if (tipo == "temperatura") {
                    encontrou_temperatura = false;
                }

                continue;
            }

            double valor = leitura.at("valor").get<double>();

            if (tipo == "nivel") {
                nivel = valor;
                encontrou_nivel = true;
            } else if (tipo == "temperatura") {
                temperatura = valor;
                encontrou_temperatura = true;
            }
        }

        if (encontrou_nivel && encontrou_temperatura) {
            double bomba_atual =
                dados["atuadores"]["bomba"].get<double>();

            double aquecedor_atual =
                dados["atuadores"]["aquecedor"].get<double>();

            ComandoAtuadores resultado;

            if (!politica.calcular(
                    nivel,
                    temperatura,
                    bomba_atual,
                    aquecedor_atual,
                    resultado)) {
                std::cerr << "Nao foi possivel calcular o controle.\n";
                return 1;
            }

            int novo_id = estado.ultimo_confirmado + 1;

            estado.preparar_comando(
                novo_id,
                resultado.bomba,
                resultado.aquecedor
            );

            estado.atualizar_ciclo(sessao, ciclo);

            if (!estado.salvar(caminho_estado)) {
                std::cerr << "Erro ao salvar o estado do controlador.\n";
                return 1;
            }
        } else {
            std::cerr << "Controle nao atualizado: leitura de nivel ou temperatura indisponivel.\n";
        }
    }

    if (estado.tem_pendente) {
        json comando = {
            {"versao", 1},
            {"sessao", sessao},
            {"id", estado.id_pendente},
            {"atuadores", {
                {"bomba", estado.bomba_pendente},
                {"aquecedor", estado.aquecedor_pendente}
            }}
        };

        if (!gravar_json(caminho_comando, comando)) {
            std::cerr << "Erro ao gravar ou recuperar o comando.\n";
            return 1;
        }
    }

    dados["alarmes"] = json::array();

    if (!gravar_json(caminho_telemetria, dados)) {
        std::cerr << "Erro ao publicar a telemetria.\n";
        return 1;
    }

    std::cout << "Controle realizado com sucesso.\n";
    return 0;
}
