#include "aquisicao.hpp"

#include <cmath>
#include <fstream>

bool Aquisicao::carregar(const std::string& caminho,
                         nlohmann::json& dados,
                         std::string& erro) const {
    std::ifstream arquivo(caminho);

    if (!arquivo.is_open()) {
        erro = "Nao foi possivel abrir o arquivo da planta.";
        return false;
    }

    try {
        arquivo >> dados;
    } catch (const nlohmann::json::exception&) {
        erro = "Arquivo JSON invalido.";
        return false;
    }

    return validar(dados, erro);
}

bool Aquisicao::validar(const nlohmann::json& dados,
                        std::string& erro) const {
    if (!dados.is_object()) {
        erro = "O documento precisa ser um objeto JSON.";
        return false;
    }

    if (!dados.contains("versao") ||
        !dados["versao"].is_number_integer() ||
        dados["versao"] != 1) {
        erro = "Versao do documento invalida.";
        return false;
    }

    if (!dados.contains("sessao") ||
        !dados["sessao"].is_string() ||
        dados["sessao"].get<std::string>().empty()) {
        erro = "Sessao invalida.";
        return false;
    }

    if (!dados.contains("ciclo") ||
        !dados["ciclo"].is_number_integer() ||
        dados["ciclo"].get<long long>() < 0) {
        erro = "Ciclo invalido.";
        return false;
    }

    if (!dados.contains("timestamp") ||
        !dados["timestamp"].is_string()) {
        erro = "Timestamp invalido.";
        return false;
    }

    if (!dados.contains("leituras") ||
        !dados["leituras"].is_array() ||
        dados["leituras"].empty()) {
        erro = "Lista de leituras invalida.";
        return false;
    }

    for (const auto& leitura : dados["leituras"]) {
        if (!leitura.is_object() ||
            !leitura.contains("tag") ||
            !leitura["tag"].is_string() ||
            !leitura.contains("tipo") ||
            !leitura["tipo"].is_string() ||
            !leitura.contains("unidade") ||
            !leitura["unidade"].is_string() ||
            !leitura.contains("status") ||
            !leitura["status"].is_string() ||
            !leitura.contains("valor") ||
            !leitura["valor"].is_number()) {
            erro = "Uma leitura possui campos invalidos.";
            return false;
        }

        double valor = leitura["valor"].get<double>();

        if (!std::isfinite(valor) || valor < 0 || valor > 100) {
            erro = "Valor de leitura fora dos limites.";
            return false;
        }
    }

    if (!dados.contains("atuadores") ||
        !dados["atuadores"].is_object() ||
        dados["atuadores"].size() != 2 ||
        !dados["atuadores"].contains("bomba") ||
        !dados["atuadores"].contains("aquecedor")) {
        erro = "Estrutura de atuadores invalida.";
        return false;
    }

    for (const auto& item : dados["atuadores"].items()) {
        if (!item.value().is_number()) {
            erro = "Valor de atuador invalido.";
            return false;
        }

        double valor = item.value().get<double>();

        if (!std::isfinite(valor) || valor < 0 || valor > 100) {
            erro = "Atuador fora dos limites.";
            return false;
        }
    }

    if (!dados.contains("ultimo_comando") ||
        !dados["ultimo_comando"].is_number_integer() ||
        dados["ultimo_comando"].get<long long>() < 0) {
        erro = "Identificador do ultimo comando invalido.";
        return false;
    }

    if (!dados.contains("erro_comando") ||
        (!dados["erro_comando"].is_null() &&
         !dados["erro_comando"].is_string())) {
        erro = "Campo erro_comando invalido.";
        return false;
    }

    if (!dados.contains("motivo_seguro") ||
        (!dados["motivo_seguro"].is_null() &&
         !dados["motivo_seguro"].is_string())) {
        erro = "Campo motivo_seguro invalido.";
        return false;
    }

    return true;
}
