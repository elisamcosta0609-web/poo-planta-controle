#include "aquisicao.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

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

    Aquisicao aquisicao;
    nlohmann::json dados;
    std::string erro;

    if (!aquisicao.carregar(caminho_planta, dados, erro)) {
        std::cerr << erro << '\n';
        return 1;
    }

    dados["alarmes"] = nlohmann::json::array();

    std::string temporario = caminho_telemetria + ".tmp";

    {
        std::ofstream arquivo(temporario);

        if (!arquivo.is_open()) {
            std::cerr << "Nao foi possivel criar a telemetria.\n";
            return 1;
        }

        arquivo << dados.dump(2) << '\n';

        if (!arquivo) {
            std::cerr << "Erro ao gravar a telemetria.\n";
            arquivo.close();
            std::filesystem::remove(temporario);
            return 1;
        }
    }

    try {
        std::filesystem::rename(temporario, caminho_telemetria);
    } catch (const std::filesystem::filesystem_error&) {
        std::filesystem::remove(temporario);
        std::cerr << "Erro ao publicar a telemetria.\n";
        return 1;
    }

    std::cout << "Aquisicao realizada com sucesso.\n";
    return 0;
}
