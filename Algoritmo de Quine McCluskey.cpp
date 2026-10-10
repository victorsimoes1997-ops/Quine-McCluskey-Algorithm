#include <iostream>
#include <vector>
#include <cmath>

struct Termo 
{
    int valor;
    int mascara;
    int qtdOnes;
    bool consumido;
};

struct Grafo
{
    int nro_vert;
    int Gmax;
    bool eh_ponderado;
    int* grau;
    int* arestas;
    float* pesos;
    Termo* termos;
};

Grafo* criarGrafo(int nro_vert, int Gmax, bool ponderado)
{
    Grafo* gr = new Grafo;
    
    gr->nro_vert = nro_vert;
    gr->Gmax = Gmax;
    gr->eh_ponderado = ponderado;
    
    gr->grau = new int[nro_vert];
    for(int i = 0; i < nro_vert; i++)
    {
        gr->grau[i] = 0;
    }
    
    gr->arestas = new int[nro_vert * Gmax];
    
    if(gr->eh_ponderado) gr->pesos = new float[nro_vert * Gmax];
    else gr->pesos = nullptr;
    
    gr->termos = new Termo[nro_vert];
    for(int i = 0; i < nro_vert; i++)
    {
        gr->termos[i].valor = 0;
        gr->termos[i].mascara = 0;
        gr->termos[i].qtdOnes = 0;
        gr->termos[i].consumido = false;
    }
    
    return gr;
}

void destruirGrafo(Grafo* gr)
{
    if(!gr) return;
    
    delete[] gr->grau;
    delete[] gr->arestas;
    
    if(gr->pesos) 
    {
        delete[] gr->pesos;
    }
    
    delete[] gr->termos;
    delete gr;
}

void inserirAresta(Grafo* gr, int origem, int destino, bool digrafo, float peso)
{
    if(!gr) return;
    if(origem < 0 || origem >= gr->nro_vert) return;
    if(destino < 0 || destino >= gr->nro_vert) return;
    if(gr->grau[origem] >= gr->Gmax) return;
    
    int indiceIda = (gr->Gmax * origem) + gr->grau[origem];
    gr->arestas[indiceIda] = destino;
    gr->grau[origem]++;
    
    if(gr->eh_ponderado) 
    {
        gr->pesos[indiceIda] = peso;
    }
    
    if(!digrafo) inserirAresta(gr, destino, origem, true, peso);
}

void imprimirGrafo(Grafo* gr)
{
    if(!gr) return;
    
    for(int v = 0; v < gr->nro_vert; v++)
    {
        std::cout << "Vértice: " << v 
                  << " [Termo -> Valor: " << gr->termos[v].valor 
                  << ", Máscara: " << gr->termos[v].mascara 
                  << ", Qtd 1s: " << gr->termos[v].qtdOnes << "]\n";
        
        for(int a = 0; a < gr->grau[v]; a++)
        {
            int indice = (v * gr->Gmax) + a; 
            int destino = gr->arestas[indice];
            
            std::cout << "  -> Aresta para: " << destino;
            
            if(gr->eh_ponderado) {
                std::cout << " (Peso: " << gr->pesos[indice] << ")";
            }
            std::cout << "\n";
        }
    }
}

void construirHipercubo(Grafo* gr)
{
    for(int i = 0; i < gr->nro_vert; i++)
    {
        for(int j = i + 1; j < gr->nro_vert; j++)
        {
            if(gr->termos[i].mascara == gr->termos[j].mascara 
            && abs(gr->termos[i].qtdOnes - gr->termos[j].qtdOnes) == 1)
            {
                int diff = gr->termos[i].valor ^ gr->termos[j].valor;
                if(diff && (diff - 1) == 0)
                {
                    inserirAresta(gr, i, j, false, 0.0f);
                    gr->termos[i].consumido = true;
                    gr->termos[j].consumido = true;
                }
            }
        }
    }
}

std::vector<Termo> identificarImplicantesPrimos(Grafo* gr) 
{
    if(!gr) return {};
    
    std::vector<Termo> implicantesPrimos;
    
    for(int i = 0; i < gr->nro_vert; i++)
    {
        if(!gr->termos[i].consumido)
        {
            implicantesPrimos.push_back(gr->termos[i]);
        }
    }
    
    return implicantesPrimos;
}

std::vector<Termo> encontrarImplicantesEssenciais(const std::vector<Termo>& implicantesPrimos, Grafo* gr) 
{
    if(!gr || implicantesPrimos.empty()) return {};
    
    int numPIs = implicantesPrimos.size();
    int numMinTerms = gr->nro_vert;
    
    std::vector<bool> minitermoCoberto(numMinTerms, false);
    std::vector<std::vector<bool>> matrizCobertura(numPIs, std::vector<bool>(numMinTerms, false));
    
    for(int i = 0; i < numPIs; i++)
    {
        for(int j = 0; j < numMinTerms; j++)
        {
            int mascaraInvertida = ~implicantesPrimos[i].mascara;
            
            if((gr->termos[j].valor & mascaraInvertida) == implicantesPrimos[i].valor)
            {
                matrizCobertura[i][j] = true;
            }
        }
    }
    
    std::vector<Termo> respostaFinal;
    bool progresso = true;
    
    while(progresso)
    {
        progresso = false;
        
        for(int j = 0; j < numMinTerms; j++)
        {
            if(minitermoCoberto[j]) continue;
            
            int cont = 0;
            int linhaUltimoPI = -1;
            
            for(int i = 0; i < numPIs; i++)
            {
                if(matrizCobertura[i][j])
                {
                    cont++;
                    linhaUltimoPI = i;
                }
            }
            
            if(cont == 1 && linhaUltimoPI != -1)
            {
                std::cout << "Implicante Primo Essencial encontrado na linha: " << linhaUltimoPI << "\n";
                respostaFinal.push_back(implicantesPrimos[linhaUltimoPI]);
                
                for(int col = 0; col < numMinTerms; col++)
                {
                    if(matrizCobertura[linhaUltimoPI][col])
                    {
                        minitermoCoberto[col] = true;
                        progresso = true;
                    }
                }
            }
        }
    }
    
    return respostaFinal;
}

int main()
{
    std::cout << "=== INICIANDO TESTE DO QUINE-MCCLUSKEY COM GRAFOS ===\n\n";

    int nro_minitermos = 4;
    int Gmax = 3; 
    bool ponderado = false;

    Grafo* gr = criarGrafo(nro_minitermos, Gmax, ponderado);

    gr->termos[0] = {0, 0, 0, false};
    gr->termos[1] = {1, 0, 1, false};
    gr->termos[2] = {4, 0, 1, false};
    gr->termos[3] = {5, 0, 2, false};

    std::cout << "[1] Grafo e Minitermos Iniciais Criados!\n";
    imprimirGrafo(gr);
    std::cout << "--------------------------------------------------\n\n";

    std::cout << "[2] Construindo o Hipercubo de Arestas (Fase 1)...\n";
    construirHipercubo(gr);
    imprimirGrafo(gr);
    std::cout << "--------------------------------------------------\n\n";

    std::cout << "[3] Identificando Implicantes Primos...\n";
    std::vector<Termo> p_is = identificarImplicantesPrimos(gr);
    std::cout << "Total de Implicantes Primos encontrados: " << p_is.size() << "\n\n";

    std::cout << "[4] Executando Tabela de Cobertura e Buscando EPIs...\n";
    std::vector<Termo> epis = encontrarImplicantesEssenciais(p_is, gr);
    std::cout << "Total de EPIs encontrados: " << epis.size() << "\n\n";

    destruirGrafo(gr);
    std::cout << "=== TESTE CONCLUÍDO COM SUCESSO! Memória liberada. ===\n";

    return 0;
}