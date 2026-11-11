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
        {"leituras", {
            {
                {"tag", "LT-01"},
                {"tipo", "nivel"},
                {"valor", 50.0},
                {"unidade", "%"},
                {"status", "operando"}
            }
        }},
        {"atuadores", {
            {"bomba", 0.0},
            {"aquecedor", 0.0}
        }},
        {"ultimo_comando", 0},
        {"erro_comando", nullptr},
        {"motivo_seguro", "sem_comando"}
    };

    {
        std::ofstream arquivo(caminho);
        arquivo << dados.dump(2);
    }

    Aquisicao aquisicao;
    nlohmann::json resultado;
    std::string erro;

    if (!aquisicao.carregar(caminho.string(), resultado, erro)) {
        std::cerr << "Falha ao aceitar dados validos: " << erro << '\n';
        std::filesystem::remove(caminho);
        return 1;
    }

    dados["leituras"][0]["valor"] = 150.0;

    {
        std::ofstream arquivo(caminho);
        arquivo << dados.dump(2);
    }

    if (aquisicao.carregar(caminho.string(), resultado, erro)) {
        std::cerr << "Falha: aceitou uma leitura fora dos limites.\n";
        std::filesystem::remove(caminho);
        return 2;
    }

    std::filesystem::remove(caminho);

    std::cout << "Testes de Aquisicao aprovados.\n";
    return 0;
}
