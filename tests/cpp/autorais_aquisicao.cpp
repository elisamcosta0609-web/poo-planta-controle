#include "aquisicao.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

int main() {
    std::filesystem::path caminho =
        std::filesystem::temp_directory_path() / "teste_aquisicao.json";

    nlohmann::json dados = {
        {"versao", 1},
        {"sessao", "teste-001"},
        {"ciclo", 1},
        {"timestamp", "2026-11-11T12:00:00Z"},
        {"leituras", {{
            {"tag", "LT-01"},
            {"tipo", "nivel"},
            {"valor", 50.0},
            {"unidade", "%"},
            {"status", "operando"}
        }}},
        {"atuadores", {
            {"bomba", 0.0},
            {"aquecedor", 0.0}
        }},
        {"ultimo_comando", 0},
        {"erro_comando", nullptr},
        {"motivo_seguro", "sem_comando"}
    };

    Aquisicao aquisicao;
    nlohmann::json resultado;
    std::string erro;

    auto gravar = [&]() {
        std::ofstream arquivo(caminho);
        arquivo << dados.dump(2);
    };

    // 1. Aceitar uma aquisição válida.
    gravar();

    if (!aquisicao.carregar(caminho.string(), resultado, erro)) {
        std::cerr << "Falha ao aceitar dados validos: " << erro << '\n';
        std::filesystem::remove(caminho);
        return 1;
    }

    if (resultado["sessao"] != "teste-001" ||
        resultado["leituras"][0]["valor"] != 50.0) {
        std::cerr << "Falha ao recuperar os dados validos.\n";
        std::filesystem::remove(caminho);
        return 2;
    }

    // Guardar o resultado válido para verificar sua preservação.
    nlohmann::json anterior = resultado;

    // 2. Rejeitar leitura acima do limite.
    dados["leituras"][0]["valor"] = 150.0;
    gravar();

    if (aquisicao.carregar(caminho.string(), resultado, erro) ||
        resultado != anterior) {
        std::cerr << "Falha ao rejeitar leitura invalida ou preservar resultado.\n";
        std::filesystem::remove(caminho);
        return 3;
    }

    // 3. Rejeitar campo obrigatório ausente.
    dados = anterior;
    dados.erase("sessao");
    gravar();

    if (aquisicao.carregar(caminho.string(), resultado, erro) ||
        resultado != anterior) {
        std::cerr << "Falha ao rejeitar sessao ausente ou preservar resultado.\n";
        std::filesystem::remove(caminho);
        return 4;
    }

    // 4. Rejeitar tipo incorreto em uma leitura.
    dados = anterior;
    dados["leituras"][0]["valor"] = "cinquenta";
    gravar();

    if (aquisicao.carregar(caminho.string(), resultado, erro) ||
        resultado != anterior) {
        std::cerr << "Falha ao rejeitar valor com tipo incorreto.\n";
        std::filesystem::remove(caminho);
        return 5;
    }

    // 5. Rejeitar atuador acima do limite.
    dados = anterior;
    dados["atuadores"]["bomba"] = 120.0;
    gravar();

    if (aquisicao.carregar(caminho.string(), resultado, erro) ||
        resultado != anterior) {
        std::cerr << "Falha ao rejeitar atuador fora dos limites.\n";
        std::filesystem::remove(caminho);
        return 6;
    }

    // 6. Rejeitar estrutura de atuadores incompleta.
    dados = anterior;
    dados["atuadores"].erase("bomba");
    gravar();

    if (aquisicao.carregar(caminho.string(), resultado, erro) ||
        resultado != anterior) {
        std::cerr << "Falha ao rejeitar atuador ausente.\n";
        std::filesystem::remove(caminho);
        return 7;
    }

    // 7. Rejeitar JSON corrompido.
    {
        std::ofstream arquivo(caminho);
        arquivo << "{ JSON invalido ";
    }

    if (aquisicao.carregar(caminho.string(), resultado, erro) ||
        resultado != anterior) {
        std::cerr << "Falha ao rejeitar JSON corrompido.\n";
        std::filesystem::remove(caminho);
        return 8;
    }

    // 8. Rejeitar arquivo inexistente.
    std::filesystem::remove(caminho);

    if (aquisicao.carregar(caminho.string(), resultado, erro) ||
        resultado != anterior) {
        std::cerr << "Falha ao rejeitar arquivo inexistente.\n";
        return 9;
    }

    std::cout << "Testes de Aquisicao aprovados.\n";
    return 0;
}
