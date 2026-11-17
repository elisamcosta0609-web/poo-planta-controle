#include "include/estado.hpp"

#include <cstdio>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

bool Estado::mesmo_ciclo(const std::string& nova_sessao,
                         int novo_ciclo) const {
    return sessao == nova_sessao && ciclo == novo_ciclo;
}

void Estado::atualizar_ciclo(const std::string& nova_sessao,
                             int novo_ciclo) {
    sessao = nova_sessao;
    ciclo = novo_ciclo;
}

void Estado::confirmar_comando(int novo_id) {
    if (novo_id > ultimo_confirmado) {
        ultimo_confirmado = novo_id;
    }

    if (tem_pendente && novo_id >= id_pendente) {
        limpar_pendente();
    }
}

void Estado::preparar_comando(int novo_id,
                              double bomba,
                              double aquecedor) {
    id_pendente = novo_id;
    bomba_pendente = bomba;
    aquecedor_pendente = aquecedor;
    tem_pendente = true;
}

void Estado::limpar_pendente() {
    tem_pendente = false;
    id_pendente = 0;
    bomba_pendente = 0.0;
    aquecedor_pendente = 0.0;
}

bool Estado::carregar(const std::string& caminho) {
    std::ifstream arquivo(caminho);

    if (!arquivo.is_open()) {
        return false;
    }

    try {
        json dados;
        arquivo >> dados;

        if (dados.at("versao").get<int>() != 2) {
            return false;
        }

        std::string nova_sessao =
            dados.at("sessao").get<std::string>();

        int novo_ciclo = dados.at("ciclo").get<int>();
        int novo_confirmado =
            dados.at("ultimo_confirmado").get<int>();

        bool novo_tem_pendente =
            dados.at("tem_pendente").get<bool>();

        int novo_id_pendente =
            dados.at("id_pendente").get<int>();

        double nova_bomba =
            dados.at("bomba_pendente").get<double>();

        double novo_aquecedor =
            dados.at("aquecedor_pendente").get<double>();

        if (nova_sessao.empty() ||
            novo_ciclo < 0 ||
            novo_confirmado < 0) {
            return false;
        }

        if (novo_tem_pendente) {
            if (novo_id_pendente <= novo_confirmado ||
                !std::isfinite(nova_bomba) ||
                !std::isfinite(novo_aquecedor) ||
                nova_bomba < 0.0 || nova_bomba > 100.0 ||
                novo_aquecedor < 0.0 || novo_aquecedor > 100.0) {
                return false;
            }
        }

        sessao = nova_sessao;
        ciclo = novo_ciclo;
        ultimo_confirmado = novo_confirmado;
        tem_pendente = novo_tem_pendente;
        id_pendente = novo_id_pendente;
        bomba_pendente = nova_bomba;
        aquecedor_pendente = novo_aquecedor;

        return true;
    } catch (...) {
        return false;
    }
}

bool Estado::salvar(const std::string& caminho) const {
    json dados = {
        {"versao", 2},
        {"sessao", sessao},
        {"ciclo", ciclo},
        {"ultimo_confirmado", ultimo_confirmado},
        {"tem_pendente", tem_pendente},
        {"id_pendente", id_pendente},
        {"bomba_pendente", bomba_pendente},
        {"aquecedor_pendente", aquecedor_pendente}
    };

    std::string temporario = caminho + ".tmp";

    {
        std::ofstream arquivo(temporario);

        if (!arquivo.is_open()) {
            return false;
        }

        arquivo << dados.dump(2) << '\n';

        if (!arquivo) {
            arquivo.close();
            std::remove(temporario.c_str());
            return false;
        }
    }

    try {
        std::filesystem::rename(temporario, caminho);
        return true;
    } catch (...) {
        std::remove(temporario.c_str());
        return false;
    }
}
