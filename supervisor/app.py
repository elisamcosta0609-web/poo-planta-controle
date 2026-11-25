"""Interface de supervisão da planta didática."""

import json
import os
from pathlib import Path

import pandas as pd
import streamlit as st

from supervisor.repositorio_telemetria import RepositorioTelemetria


def main():
    dados = Path(os.environ.get("POO_DADOS", "dados"))
    caminho = dados / "telemetria.json"

    st.title("Supervisório da planta didática")
    st.caption(f"Diretório de integração: {dados}")

    repositorio = RepositorioTelemetria(caminho)

    try:
        telemetria = repositorio.carregar()

    except FileNotFoundError:
        st.warning("Arquivo de telemetria não encontrado.")
        return

    except json.JSONDecodeError:
        st.error("Arquivo de telemetria contém JSON inválido.")
        return

    except ValueError as erro:
        st.error(str(erro))
        return

    except OSError as erro:
        st.error(f"Erro ao ler telemetria: {erro}")
        return

    leituras = telemetria.get("leituras", [])

    st.subheader("Leituras dos sensores")

    tabela = pd.DataFrame(leituras)
    st.dataframe(tabela, use_container_width=True)

    st.subheader("Alarmes")
    alarmes = telemetria.get("alarmes", [])

    if alarmes:
        for alarme in alarmes:
            st.warning(str(alarme))
    else:
        st.success("Nenhum alarme registrado.")

    st.subheader("Informações da planta")
    st.write("Sessão:", telemetria.get("sessao", "Não informada"))
    st.write("Ciclo:", telemetria.get("ciclo", "Não informado"))

    st.subheader("Estado dos atuadores")
    st.json(telemetria.get("atuadores", {}))


if __name__ == "__main__":
    main()
