#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// ??? Estrutura de Dados do Produto
// Utiliza campos maiores para acomodar textos maiores, conforme especificado.
#define MAX_NOME 100
#define MAX_CATEGORIA 50
#define MAX_PRODUTOS 50 // Limite de produtos na memória

typedef struct {
    int id;
    char nome[MAX_NOME];
    float preco;
    char categoria[MAX_CATEGORIA];
} Produto;


Produto lista_produtos[MAX_PRODUTOS];
int total_produtos = 0;
int proximo_id = 1;




// Transforma uma string em letras minúsculas para comparação não-sensível ao caso.
void to_lower_case(char *dest, const char *src) {
    int i;
    for (i = 0; src[i] != '\0' && i < MAX_NOME - 1; i++) {
        dest[i] = tolower((unsigned char)src[i]);
    }
    dest[i] = '\0';
}

// Verifica se um nome de produto já existe , retornando 1 se duplicado e
// caso contrário.
int verificar_duplicata(const char *nome_novo) {
    char nome_novo_lower[MAX_NOME];
    char nome_existente_lower[MAX_NOME];

    to_lower_case(nome_novo_lower, nome_novo);

    for (int i = 0; i < total_produtos; i++) {
        to_lower_case(nome_existente_lower, lista_produtos[i].nome);
        if (strcmp(nome_existente_lower, nome_novo_lower) == 0) {
            return 1; // Duplicata encontrada
        }
    }
    return 0;
}

// Gera o próximo ID incremental e o atualiza
int gerar_proximo_id() {
    return proximo_id++;
}


// Funções de Arquivo
// Carrega os dados do arquivo para a estrutura de dados (vetor)
void carregar_dados() {
    FILE *arquivo = fopen("dados_produtos.txt", "r");
    total_produtos = 0;
    proximo_id = 1;

    if (arquivo == NULL) {
        // Se o arquivo não existir, será criado na primeira gravação
        return;
    }

    // Lê linha por linha e preenche o vetor de estruturas
    while (total_produtos < MAX_PRODUTOS &&
           fscanf(arquivo, "%d;%99[^;];%f;%49[^\n]\n", 
                  &lista_produtos[total_produtos].id, 
                  lista_produtos[total_produtos].nome, 
                  &lista_produtos[total_produtos].preco, 
                  lista_produtos[total_produtos].categoria) == 4) {
        
        // Atualiza o próximo ID garantindo a incrementalidade
        if (lista_produtos[total_produtos].id >= proximo_id) {
            proximo_id = lista_produtos[total_produtos].id + 1;
        }
        total_produtos++;
    }

    fclose(arquivo);
}

// Salva os dados da estrutura de dados (vetor) de volta para o arquivo
void salvar_dados() {
    FILE *arquivo = fopen("dados_produtos.txt", "w");

    if (arquivo == NULL) {
        printf("Erro: Não foi possível abrir o arquivo para salvar!\n");
        return;
    }

    // Escreve cada produto no arquivo no formato: ID;NOME;PRECO;CATEGORIA
    for (int i = 0; i < total_produtos; i++) {
        fprintf(arquivo, "%d;%s;%.2f;%s\n", 
                lista_produtos[i].id, 
                lista_produtos[i].nome, 
                lista_produtos[i].preco, 
                lista_produtos[i].categoria);
    }

    fclose(arquivo);
}


// Funções CRUD
// Cria e adiciona um novo produto
void criar_produto() {
    if (total_produtos >= MAX_PRODUTOS) {
        printf(" Limite máximo de produtos (%d) atingido!\n", MAX_PRODUTOS);
        return;
    }

    Produto novoProduto;
    char temp_nome[MAX_NOME];

    printf("\n--- Criar Novo Produto ---\n");
    novoProduto.id = gerar_proximo_id(); // Geração automática de ID

    // Solicita nome e verifica duplicatas
    do {
        printf("Nome: ");
        // Limpa o buffer antes de ler a string com espacos
        scanf(" %99[^\n]", temp_nome); 
        strcpy(novoProduto.nome, temp_nome);
        
        if (verificar_duplicata(novoProduto.nome)) {
            printf("Erro: Produto com este nome já cadastrado. Tente novamente.\n");
        }
    } while (verificar_duplicata(novoProduto.nome));

    printf("Preço (ex: 12.99): ");
    scanf("%f", &novoProduto.preco);
    
    printf("Categoria: ");
    scanf(" %49[^\n]", novoProduto.categoria);

    // Adiciona ao vetor
    lista_produtos[total_produtos] = novoProduto;
    total_produtos++;
    
    // Salva imediatamente
    salvar_dados();
    
    printf("Produto '%s' (ID: %d) cadastrado com sucesso!\n", novoProduto.nome, novoProduto.id);
}

// Busca produto por ID, nome (parcial) ou categoria (parcial)
void buscar_produto() {
    int opcao;
    printf("\n--- Buscar Produto ---\n");
    printf("1 - Buscar por ID\n");
    printf("2 - Buscar por Nome\n");
    printf("3 - Buscar por Categoria\n");
    printf("Escolha a opção: ");
    scanf("%d", &opcao);

    char termo_busca[MAX_NOME];
    char termo_busca_lower[MAX_NOME];
    int encontrados = 0;

    printf("Digite o termo de busca: ");
    scanf(" %99[^\n]", termo_busca);
    to_lower_case(termo_busca_lower, termo_busca);

    printf("\n--- Resultados da Busca ---\n");
    printf("+------+--------------------------------+------------+--------------------+\n");
    printf("|  ID  | NOME                           | PREÇO      | CATEGORIA          |\n");
    printf("+------+--------------------------------+------------+--------------------+\n");
    
    for (int i = 0; i < total_produtos; i++) {
        int corresponde = 0;
        
        // Comparações e buscas 'case-insensitive'
        if (opcao == 1) { // Por ID
            int id_busca = atoi(termo_busca); // Converte o termo para int
            if (lista_produtos[i].id == id_busca) {
                corresponde = 1;
            }
        } else if (opcao == 2) { // Por Nome (Busca parcial)
            char nome_produto_lower[MAX_NOME];
            to_lower_case(nome_produto_lower, lista_produtos[i].nome);
            if (strstr(nome_produto_lower, termo_busca_lower) != NULL) {
                corresponde = 1;
            }
        } else if (opcao == 3) { // Por Categoria (Busca parcial)
            char cat_produto_lower[MAX_CATEGORIA];
            to_lower_case(cat_produto_lower, lista_produtos[i].categoria);
            if (strstr(cat_produto_lower, termo_busca_lower) != NULL) {
                corresponde = 1;
            }
        }
        
        if (corresponde) {
            printf("| %-4d | %-30.30s | R$ %-7.2f | %-18.18s |\n",
                   lista_produtos[i].id, 
                   lista_produtos[i].nome, 
                   lista_produtos[i].preco, 
                   lista_produtos[i].categoria);
            encontrados++;
        }
    }
    printf("+------+--------------------------------+------------+--------------------+\n");

    if (encontrados == 0) {
        printf("| Nenhum produto encontrado.                                                              |\n");
        printf("+-----------------------------------------------------------------------------------------+\n");
    }
}

// Altera um produto existente
void alterar_produto() {
    int id_alterar, indice = -1;
    printf("\n--- Alterar Produto ---\n");
    printf("Digite o ID do produto que deseja alterar: ");
    scanf("%d", &id_alterar);

    // Encontra o índice do produto no vetor
    for (int i = 0; i < total_produtos; i++) {
        if (lista_produtos[i].id == id_alterar) {
            indice = i;
            break;
        }
    }

    if (indice == -1) {
        printf("Erro: Produto com ID %d não encontrado.\n", id_alterar);
        return;
    }

    printf("Alterando: %s (ID: %d)\n", lista_produtos[indice].nome, lista_produtos[indice].id);

    printf("Novo Nome (Atual: %s): ", lista_produtos[indice].nome);
    char novo_nome[MAX_NOME];
    scanf(" %99[^\n]", novo_nome);

    // Verifica duplicata, ignorando o próprio nome atual se ele for digitado
    if (strcmp(novo_nome, lista_produtos[indice].nome) != 0 && verificar_duplicata(novo_nome)) {
        printf("Erro: Produto com o nome '%s' já existe. Alteração cancelada.\n", novo_nome);
        return;
    }
    strcpy(lista_produtos[indice].nome, novo_nome);

    printf("Novo Preço (Atual: %.2f): ", lista_produtos[indice].preco);
    scanf("%f", &lista_produtos[indice].preco);
    
    printf("Nova Categoria (Atual: %s): ", lista_produtos[indice].categoria);
    scanf(" %49[^\n]", lista_produtos[indice].categoria);

    // Salva usando o mecanismo de arquivo temporário
    // Ao salvar, a função `salvar_dados` reescreve todo o arquivo
    salvar_dados(); 
    
    printf("Produto ID %d alterado com sucesso!\n", id_alterar);
}

// Exclui um produto existente
void excluir_produto() {
    int id_excluir, indice = -1;
    printf("\n--- Excluir Produto ---\n");
    printf("Digite o ID do produto que deseja excluir: ");
    scanf("%d", &id_excluir);

    // Encontra o índice do produto no vetor
    for (int i = 0; i < total_produtos; i++) {
        if (lista_produtos[i].id == id_excluir) {
            indice = i;
            break;
        }
    }

    if (indice == -1) {
        printf("Erro: Produto com ID %d não encontrado.\n", id_excluir);
        return;
    }

    // Move os elementos seguintes uma posição para trás (exclusão lógica)
    for (int i = indice; i < total_produtos - 1; i++) {
        lista_produtos[i] = lista_produtos[i+1];
    }
    total_produtos--;

    // Salva usando o mecanismo de arquivo temporário
    // Ao salvar, a função `salvar_dados` reescreve todo o arquivo
    salvar_dados(); 
    
    printf("Produto ID %d excluído com sucesso!\n", id_excluir);
}

// Funções de Ordenação
// Função de troca para Bubble Sort
void trocar(Produto *a, Produto *b) {
    Produto temp = *a;
    *a = *b;
    *b = temp;
}

// Ordenação por noma (A - Z)
void ordenar_nome_az() {
    int i, j;
    for (i = 0; i < total_produtos - 1; i++) {
        for (j = 0; j < total_produtos - i - 1; j++) {
            // Compara strings para ordenação
            if (strcmp(lista_produtos[j].nome, lista_produtos[j+1].nome) > 0) {
                trocar(&lista_produtos[j], &lista_produtos[j+1]);
            }
        }
    }
    printf("Lista de produtos ordenada por Nome (A-Z).\n");
}

// Bubble Sort para ordenar por Nome (Z-A)
void ordenar_nome_za() {
    int i, j;
    for (i = 0; i < total_produtos - 1; i++) {
        for (j = 0; j < total_produtos - i - 1; j++) {
            // Compara strings para ordenação (ordem inversa)
            if (strcmp(lista_produtos[j].nome, lista_produtos[j+1].nome) < 0) {
                trocar(&lista_produtos[j], &lista_produtos[j+1]);
            }
        }
    }
    printf("Lista de produtos ordenada por Nome (Z-A).\n");
}

// Bubble Sort para ordenar por Preço (menor -> maior)
void ordenar_preco_menor() {
    int i, j;
    for (i = 0; i < total_produtos - 1; i++) {
        for (j = 0; j < total_produtos - i - 1; j++) {
            // Compara floats
            if (lista_produtos[j].preco > lista_produtos[j+1].preco) {
                trocar(&lista_produtos[j], &lista_produtos[j+1]);
            }
        }
    }
    printf("Lista de produtos ordenada por Preço (Menor -> Maior).\n");
}

// Bubble Sort para ordenar por Preço (maior -> menor)
void ordenar_preco_maior() {
    int i, j;
    for (i = 0; i < total_produtos - 1; i++) {
        for (j = 0; j < total_produtos - i - 1; j++) {
            // Compara floats (ordem inversa)
            if (lista_produtos[j].preco < lista_produtos[j+1].preco) {
                trocar(&lista_produtos[j], &lista_produtos[j+1]);
            }
        }
    }
    printf("Lista de produtos ordenada por Preço (Maior -> Menor).\n");
}

//Funções de Exibição e Exportação
// Exibe o resultado em formato de tabela
void listar_produtos() {
    printf("\n--- Lista de Produtos (%d cadastrados) ---\n", total_produtos);
    printf("+------+--------------------------------+------------+--------------------+\n");
    printf("|  ID  | NOME                           | PREÇO      | CATEGORIA          |\n");
    printf("+------+--------------------------------+------------+--------------------+\n");

    if (total_produtos == 0) {
        printf("| Nenhum produto cadastrado.                                                              |\n");
        printf("+-----------------------------------------------------------------------------------------+\n");
        return;
    }

    for (int i = 0; i < total_produtos; i++) {
        printf("| %-4d | %-30.30s | R$ %-7.2f | %-18.18s |\n",
               lista_produtos[i].id, 
               lista_produtos[i].nome, 
               lista_produtos[i].preco, 
               lista_produtos[i].categoria);
    }
    printf("+------+--------------------------------+------------+--------------------+\n");
}

// Exporta a lista atual para CSV
void exportar_csv() {
    FILE *arquivo = fopen("produtos_export.csv", "w");

    if (arquivo == NULL) {
        printf("?? Erro: Não foi possível criar o arquivo CSV!\n");
        return;
    }
    //Cabeçalho
    fprintf(arquivo, "ID,Nome,Preco,Categoria\n");
    
    //Dados
    for (int i = 0; i < total_produtos; i++) {
        fprintf(arquivo, "%d,\"%s\",%.2f,\"%s\"\n", 
                lista_produtos[i].id, 
                lista_produtos[i].nome, 
                lista_produtos[i].preco, 
                lista_produtos[i].categoria);
    }

    fclose(arquivo);
    printf("Produtos exportados para 'produtos_export.csv' com sucesso!\n");
}


// Função principal
int main() {
    // Carrega os dados existentes ao iniciar
    carregar_dados();

    int escolha;

    do {
        printf("\n========== Sistema de Produtos ==========\n");
        printf("1. Criar novo produto (ID auto, checa duplicata)\n");
        printf("2. Listar produtos (Tabela)\n");
        printf("3. Buscar produto (por Nome/Categoria/ID)\n");
        printf("4. Alterar produto (Salva c/ arquivo temporário)\n");
        printf("5. Excluir produto (Salva c/ arquivo temporário)\n");
        printf("--- Ordenação (Bubble Sort) ---\n");
        printf("6. Ordenar por Nome (A-Z)\n");
        printf("7. Ordenar por Nome (Z-A)\n");
        printf("8. Ordenar por Preço (Menor -> Maior)\n");
        printf("9. Ordenar por Preço (Maior -> Menor)\n");
        printf("10. Exportar para CSV\n");
        printf("0. Sair\n");
        printf("-----------------------------------------\n");
        printf("Escolha a opção: ");
        
        // Garante que o buffer de entrada seja limpo após a leitura do int
        if (scanf("%d", &escolha) != 1) {
             // Tratamento simples para entrada inválida (não numérica)
             int c;
             while ((c = getchar()) != '\n' && c != EOF)
             escolha = -1; 
        }


        switch (escolha) {
            case 1: criar_produto(); break;
            case 2: listar_produtos(); break;
            case 3: buscar_produto(); break;
            case 4: alterar_produto(); break;
            case 5: excluir_produto(); break;
            case 6: ordenar_nome_az(); listar_produtos(); break;
            case 7: ordenar_nome_za(); listar_produtos(); break;
            case 8: ordenar_preco_menor(); listar_produtos(); break;
            case 9: ordenar_preco_maior(); listar_produtos(); break;
            case 10: exportar_csv(); break;
            case 0: printf("Programa encerrado.\n"); break;
            default: printf("Opção inválida. Tente novamente.\n");
        }
        
    } while (escolha != 0);

    return 0;
}
