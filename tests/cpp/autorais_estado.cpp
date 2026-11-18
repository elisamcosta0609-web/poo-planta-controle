#include "include/estado.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>

int main() {
    std::filesystem::path caminho =
        std::filesystem::temp_directory_path() /
        "teste_estado_controlador.json";

    std::filesystem::path caminho_invalido =
        std::filesystem::temp_directory_path() /
        "teste_estado_invalido_controlador.json";

    Estado estado;

    estado.sessao = "sessao-001";
    estado.atualizar_ciclo("sessao-001", 5);
    estado.ultimo_confirmado = 2;
    estado.preparar_comando(3, 60.0, 40.0);

    if (!estado.salvar(caminho.string())) {
        std::cerr << "Falha ao salvar o estado.\n";
        return 1;
    }

    Estado recuperado;

    if (!recuperado.carregar(caminho.string())) {
        std::cerr << "Falha ao recuperar o estado.\n";
        std::filesystem::remove(caminho);
        return 2;
    }

    if (!recuperado.tem_pendente ||
        recuperado.id_pendente != 3 ||
        recuperado.bomba_pendente != 60.0 ||
        recuperado.aquecedor_pendente != 40.0) {
        std::cerr << "Falha ao recuperar o comando pendente.\n";
        std::filesystem::remove(caminho);
        return 3;
    }

    if (!recuperado.mesmo_ciclo("sessao-001", 5)) {
        std::cerr << "Falha ao reconhecer o ciclo salvo.\n";
        std::filesystem::remove(caminho);
        return 4;
    }

    recuperado.confirmar_comando(2);

    if (!recuperado.tem_pendente) {
        std::cerr << "Falha: comando foi removido antes da confirmacao.\n";
        std::filesystem::remove(caminho);
        return 5;
    }

    recuperado.confirmar_comando(3);

    if (recuperado.tem_pendente ||
        recuperado.ultimo_confirmado != 3) {
        std::cerr << "Falha ao confirmar o comando.\n";
        std::filesystem::remove(caminho);
        return 6;
    }

    recuperado.atualizar_ciclo("sessao-002", 1);

    if (recuperado.mesmo_ciclo("sessao-001", 5) ||
        !recuperado.mesmo_ciclo("sessao-002", 1)) {
        std::cerr << "Falha ao reconhecer a nova sessao.\n";
        std::filesystem::remove(caminho);
        return 7;
    }

    // Preparar um estado que deverá ser preservado.
    Estado preservado;
    preservado.sessao = "sessao-preservada";
    preservado.atualizar_ciclo("sessao-preservada", 9);
    preservado.ultimo_confirmado = 7;
    preservado.preparar_comando(8, 25.0, 35.0);

    // Testar a rejeição de um arquivo JSON corrompido.
    {
        std::ofstream arquivo(caminho_invalido);
        arquivo << "{ JSON invalido ";
    }

    if (preservado.carregar(caminho_invalido.string())) {
        std::cerr << "Falha: aceitou um JSON corrompido.\n";
        return 8;
    }

    if (preservado.sessao != "sessao-preservada" ||
        preservado.ciclo != 9 ||
        preservado.ultimo_confirmado != 7 ||
        !preservado.tem_pendente ||
        preservado.id_pendente != 8 ||
        preservado.bomba_pendente != 25.0 ||
        preservado.aquecedor_pendente != 35.0) {
        std::cerr << "Falha: alterou o estado apos JSON corrompido.\n";
        return 9;
    }

    // Testar um JSON válido na sintaxe, mas incoerente nos dados.
    {
        std::ofstream arquivo(caminho_invalido);
        arquivo << R"({
            "versao": 2,
            "sessao": "sessao-invalida",
            "ciclo": 4,
            "ultimo_confirmado": 7,
            "tem_pendente": true,
            "id_pendente": 7,
            "bomba_pendente": 50.0,
            "aquecedor_pendente": 50.0
        })";
    }

    if (preservado.carregar(caminho_invalido.string())) {
        std::cerr << "Falha: aceitou comando pendente ja confirmado.\n";
        return 10;
    }

    if (preservado.sessao != "sessao-preservada" ||
        preservado.ciclo != 9 ||
        preservado.ultimo_confirmado != 7 ||
        !preservado.tem_pendente ||
        preservado.id_pendente != 8 ||
        preservado.bomba_pendente != 25.0 ||
        preservado.aquecedor_pendente != 35.0) {
        std::cerr << "Falha: alterou o estado apos dados incoerentes.\n";
        return 11;
    }

    std::filesystem::remove(caminho);
    std::filesystem::remove(caminho_invalido);

    std::cout << "Testes de Estado aprovados.\n";
    return 0;
}
