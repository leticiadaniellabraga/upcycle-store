#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>

/* ========================================================================
   CONSTANTES E DEFINIÇÕES GLOBAIS
   ======================================================================== */

#define MAX_PRODUTOS     20      /* itens por pedido (reduzido para economia) */
#define MAX_CLIENTES     50
#define VENDAS_POR_DIA   50
#define DIAS_NO_ANO      365
#define MESES_NO_ANO     12
#define MAX_NOME         100
#define MAX_DESCRICAO    200

/* ========================================================================
   ESTRUTURAS DE DADOS (STRUCTS)
   ======================================================================== */

/* Produto */
typedef struct {
    int id;
    char nome[MAX_NOME];
    char descricao[MAX_DESCRICAO];
    float preco;
    int estoque;
    char categoria[MAX_NOME];
    int fornecedor_id;
} Produto;

/* Cliente */
typedef struct {
    int id;
    char nome[MAX_NOME];
    char email[MAX_NOME];
    char telefone[20];
    int pontos_fidelidade;
    int materiais_reciclados_entregues;
} Cliente;

/* Item do pedido – mantém contagem de devoluções por item */
typedef struct {
    int produto_id;
    int quantidade;
    float preco_unitario;
    float valor_total_item;
    int numero_devolucoes;    /* quantas vezes este item foi devolvido */
} ItemPedido;

/* Pedido – inclui contador de devoluções e flag de taxa aplicada */
typedef struct {
    int id;
    int cliente_id;
    int dia, mes, ano;
    int qtd_itens;
    ItemPedido itens[MAX_PRODUTOS];
    float valor_total_pedido;
    float taxa_transportadora;     /* R$ 20,00 quando aplicada */
    int taxa_aplicada;             /* 0 = não cobrada, 1 = já cobrada */
} Pedido;

/* Fornecedor */
typedef struct {
    int id;
    char nome[MAX_NOME];
    char tipo[MAX_NOME];
    char endereco[MAX_NOME];
    char contato[MAX_NOME];
} Fornecedor;

/* ========================================================================
   VARIÁVEIS GLOBAIS (ARRAYS)
   ======================================================================== */

Produto produtos[MAX_PRODUTOS];
int total_produtos = 0;

Cliente clientes[MAX_CLIENTES];
int total_clientes = 0;

Fornecedor fornecedores[MAX_PRODUTOS];
int total_fornecedores = 0;

Pedido pedidos[VENDAS_POR_DIA * DIAS_NO_ANO];
int total_pedidos = 0;

/* ========================================================================
   PROTÓTIPOS
   ======================================================================== */

void inicializar_sistema(void);
void cadastrar_produto(void);
void cadastrar_cliente(void);
void cadastrar_fornecedor(void);
void listar_produtos(void);
void listar_clientes(void);
void listar_fornecedores(void);
void registrar_venda(void);
void registrar_devolucao(void);
void relatorio_diario(void);
void relatorio_mensal(void);
void relatorio_anual(void);

int buscar_produto_por_id(int id);
int buscar_cliente_por_id(int id);
int contar_vendas_no_dia(int dia, int mes, int ano);
float calcular_valor_item(ItemPedido item);
void ordenar_por_faturamento(float valores[], int meses[], int tamanho);

int validar_data(int dia, int mes, int ano);
int validar_preco(float preco);
int validar_quantidade(int qtd);
int validar_id(int id, int max);

void limpar_buffer(void);
void pausar(void);
void exibir_menu(void);

/* ========================================================================
   FUNÇÃO PRINCIPAL
   ======================================================================== */

int main() {
    setlocale(LC_ALL, "Portuguese");
    inicializar_sistema();

    int opcao;
    do {
        exibir_menu();
        printf("Escolha uma opção: ");
        if (scanf("%d", &opcao) != 1) {
            printf("\nEntrada inválida! Digite um número.\n");
            limpar_buffer();
            pausar();
            continue;
        }
        limpar_buffer();

        switch (opcao) {
            case 1:  cadastrar_produto();     break;
            case 2:  cadastrar_cliente();     break;
            case 3:  cadastrar_fornecedor();  break;
            case 4:  registrar_venda();       break;
            case 5:  registrar_devolucao();   break;
            case 6:  listar_produtos();       break;
            case 7:  listar_clientes();       break;
            case 8:  listar_fornecedores();   break;
            case 9:  relatorio_diario();      break;
            case 10: relatorio_mensal();      break;
            case 11: relatorio_anual();       break;
            case 0:
                printf("\nEncerrando o sistema... Obrigado por usar a Upcycle Store!\n");
                break;
            default:
                printf("\nOpção inválida! Tente novamente.\n");
        }
        if (opcao != 0) pausar();
    } while (opcao != 0);

    return 0;
}

/* ========================================================================
   INICIALIZAÇÃO COM DADOS DE EXEMPLO
   ======================================================================== */

void inicializar_sistema() {
    printf("\n===========================================================\n");
    printf("     BEM-VINDO À UPYCLE STORE - LOJA SUSTENTÁVEL\n");
    printf("===========================================================\n");
    printf("Inicializando o sistema com dados de exemplo...\n");

    /* ---------- FORNECEDORES ---------- */
    fornecedores[0].id = 1;
    strcpy(fornecedores[0].nome, "CoopRecicla - Cooperativa de Reciclagem");
    strcpy(fornecedores[0].tipo, "Cooperativa de Reciclagem");
    strcpy(fornecedores[0].endereco, "Rua da Reciclagem, 100 - Centro");
    strcpy(fornecedores[0].contato, "(21) 99999-1111");
    total_fornecedores++;

    fornecedores[1].id = 2;
    strcpy(fornecedores[1].nome, "Tecidos Sustentáveis Silva");
    strcpy(fornecedores[1].tipo, "Produtor Local");
    strcpy(fornecedores[1].endereco, "Rua das Oficinas, 45 - Bairro Industrial");
    strcpy(fornecedores[1].contato, "(21) 98888-2222");
    total_fornecedores++;

    fornecedores[2].id = 3;
    strcpy(fornecedores[2].nome, "ArteSustentável - Cooperativa de Artesãos");
    strcpy(fornecedores[2].tipo, "Cooperativa de Reciclagem");
    strcpy(fornecedores[2].endereco, "Av. das Artes, 200 - Vila Verde");
    strcpy(fornecedores[2].contato, "(21) 97777-3333");
    total_fornecedores++;

    /* ---------- PRODUTOS ---------- */
    produtos[0].id = 1;
    strcpy(produtos[0].nome, "Camiseta Upcycled");
    strcpy(produtos[0].descricao, "Camiseta feita com tecidos reaproveitados");
    produtos[0].preco = 49.90;
    produtos[0].estoque = 100;
    strcpy(produtos[0].categoria, "Roupas");
    produtos[0].fornecedor_id = 2;
    total_produtos++;

    produtos[1].id = 2;
    strcpy(produtos[1].nome, "Bolsa EcoBag");
    strcpy(produtos[1].descricao, "Bolsa confeccionada a partir de banners reciclados");
    produtos[1].preco = 79.90;
    produtos[1].estoque = 50;
    strcpy(produtos[1].categoria, "Acessórios");
    produtos[1].fornecedor_id = 1;
    total_produtos++;

    produtos[2].id = 3;
    strcpy(produtos[2].nome, "Canudo de Bambu");
    strcpy(produtos[2].descricao, "Canudo ecológico reutilizável de bambu");
    produtos[2].preco = 12.50;
    produtos[2].estoque = 200;
    strcpy(produtos[2].categoria, "Ecológicos");
    produtos[2].fornecedor_id = 2;
    total_produtos++;

    produtos[3].id = 4;
    strcpy(produtos[3].nome, "Garrafa Térmica Eco");
    strcpy(produtos[3].descricao, "Garrafa térmica de aço inoxidável");
    produtos[3].preco = 89.90;
    produtos[3].estoque = 30;
    strcpy(produtos[3].categoria, "Ecológicos");
    produtos[3].fornecedor_id = 1;
    total_produtos++;

    produtos[4].id = 5;
    strcpy(produtos[4].nome, "Móbile Sustentável");
    strcpy(produtos[4].descricao, "Móbile decorativo com materiais reciclados");
    produtos[4].preco = 45.00;
    produtos[4].estoque = 20;
    strcpy(produtos[4].categoria, "Decoração");
    produtos[4].fornecedor_id = 3;
    total_produtos++;

    produtos[5].id = 6;
    strcpy(produtos[5].nome, "Necessaire Reciclada");
    strcpy(produtos[5].descricao, "Necessaire de retalhos de tecido");
    produtos[5].preco = 34.90;
    produtos[5].estoque = 60;
    strcpy(produtos[5].categoria, "Acessórios");
    produtos[5].fornecedor_id = 3;
    total_produtos++;

    /* ---------- CLIENTES ---------- */
    clientes[0].id = 1;
    strcpy(clientes[0].nome, "Ana Paula Souza");
    strcpy(clientes[0].email, "ana.souza@email.com");
    strcpy(clientes[0].telefone, "(21) 91234-5678");
    clientes[0].pontos_fidelidade = 150;
    clientes[0].materiais_reciclados_entregues = 5;
    total_clientes++;

    clientes[1].id = 2;
    strcpy(clientes[1].nome, "Carlos Eduardo Lima");
    strcpy(clientes[1].email, "carlos.lima@email.com");
    strcpy(clientes[1].telefone, "(21) 98765-4321");
    clientes[1].pontos_fidelidade = 80;
    clientes[1].materiais_reciclados_entregues = 2;
    total_clientes++;

    clientes[2].id = 3;
    strcpy(clientes[2].nome, "Mariana Oliveira Costa");
    strcpy(clientes[2].email, "mariana.costa@email.com");
    strcpy(clientes[2].telefone, "(21) 99876-5432");
    clientes[2].pontos_fidelidade = 0;
    clientes[2].materiais_reciclados_entregues = 0;
    total_clientes++;

    printf("Sistema inicializado com %d produtos, %d clientes e %d fornecedores.\n",
           total_produtos, total_clientes, total_fornecedores);
    printf("===========================================================\n");
}

/* ========================================================================
   MENU PRINCIPAL
   ======================================================================== */

void exibir_menu() {
    printf("\n");
    printf("===========================================================\n");
    printf("              UPYCLE STORE - SISTEMA DE VENDAS\n");
    printf("===========================================================\n");
    printf("  [1] Cadastrar Produto\n");
    printf("  [2] Cadastrar Cliente\n");
    printf("  [3] Cadastrar Fornecedor\n");
    printf("  [4] Registrar Venda\n");
    printf("  [5] Registrar Devolução de Item\n");
    printf("  [6] Listar Produtos\n");
    printf("  [7] Listar Clientes\n");
    printf("  [8] Listar Fornecedores\n");
    printf("  [9] RELATÓRIO DIÁRIO\n");
    printf(" [10] RELATÓRIO MENSAL\n");
    printf(" [11] RELATÓRIO ANUAL\n");
    printf("  [0] Sair\n");
    printf("===========================================================\n");
}

/* ========================================================================
   FUNÇÕES UTILITÁRIAS
   ======================================================================== */

void limpar_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
}

void pausar() {
    printf("\nPressione Enter para continuar...");
    limpar_buffer();
    getchar();
}

/* ========================================================================
   FUNÇÕES DE VALIDAÇÃO
   ======================================================================== */

int validar_data(int dia, int mes, int ano) {
    if (ano < 2000 || ano > 2100) return 0;
    if (mes < 1 || mes > 12) return 0;

    int dias_por_mes[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (ano % 400 == 0 || (ano % 4 == 0 && ano % 100 != 0))
        dias_por_mes[1] = 29;

    if (dia < 1 || dia > dias_por_mes[mes - 1]) return 0;
    return 1;
}

int validar_preco(float preco) {
    return (preco > 0);
}

int validar_quantidade(int qtd) {
    return (qtd > 0);
}

int validar_id(int id, int max) {
    return (id >= 0 && id <= max);
}

/* ========================================================================
   FUNÇÕES DE CADASTRO E LISTAGEM
   ======================================================================== */

void cadastrar_produto() {
    if (total_produtos >= MAX_PRODUTOS) {
        printf("\nLimite máximo de produtos atingido (%d).\n", MAX_PRODUTOS);
        return;
    }

    Produto novo;
    novo.id = total_produtos + 1;

    printf("\n--- CADASTRO DE PRODUTO ---\n");
    printf("Nome: ");
    fgets(novo.nome, MAX_NOME, stdin);
    novo.nome[strcspn(novo.nome, "\n")] = '\0';

    printf("Descrição: ");
    fgets(novo.descricao, MAX_DESCRICAO, stdin);
    novo.descricao[strcspn(novo.descricao, "\n")] = '\0';

    do {
        printf("Preço unitário (R$): ");
        if (scanf("%f", &novo.preco) != 1 || !validar_preco(novo.preco)) {
            printf("Preço inválido! Digite um valor maior que zero.\n");
            limpar_buffer();
            novo.preco = -1;
        } else {
            limpar_buffer();
            break;
        }
    } while (1);

    do {
        printf("Quantidade em estoque: ");
        if (scanf("%d", &novo.estoque) != 1 || !validar_quantidade(novo.estoque)) {
            printf("Quantidade inválida! Digite um valor maior que zero.\n");
            limpar_buffer();
            novo.estoque = -1;
        } else {
            limpar_buffer();
            break;
        }
    } while (1);

    printf("Categoria: ");
    fgets(novo.categoria, MAX_NOME, stdin);
    novo.categoria[strcspn(novo.categoria, "\n")] = '\0';

    if (total_fornecedores == 0) {
        printf("\nNenhum fornecedor cadastrado. Cadastre um fornecedor primeiro.\n");
        return;
    }

    printf("\nFornecedores disponíveis:\n");
    for (int i = 0; i < total_fornecedores; i++)
        printf("  [%d] %s\n", fornecedores[i].id, fornecedores[i].nome);

    do {
        printf("ID do fornecedor: ");
        if (scanf("%d", &novo.fornecedor_id) != 1) {
            printf("Entrada inválida!\n");
            limpar_buffer();
            continue;
        }
        limpar_buffer();
        int encontrado = 0;
        for (int i = 0; i < total_fornecedores; i++) {
            if (fornecedores[i].id == novo.fornecedor_id) {
                encontrado = 1;
                break;
            }
        }
        if (!encontrado) printf("Fornecedor não encontrado!\n");
        else break;
    } while (1);

    produtos[total_produtos++] = novo;
    printf("\nProduto cadastrado com sucesso! ID: %d\n", novo.id);
}

void cadastrar_cliente() {
    if (total_clientes >= MAX_CLIENTES) {
        printf("\nLimite máximo de clientes atingido (%d).\n", MAX_CLIENTES);
        return;
    }

    Cliente novo;
    novo.id = total_clientes + 1;

    printf("\n--- CADASTRO DE CLIENTE ---\n");
    printf("Nome completo: ");
    fgets(novo.nome, MAX_NOME, stdin);
    novo.nome[strcspn(novo.nome, "\n")] = '\0';

    printf("E-mail: ");
    fgets(novo.email, MAX_NOME, stdin);
    novo.email[strcspn(novo.email, "\n")] = '\0';

    printf("Telefone: ");
    fgets(novo.telefone, 20, stdin);
    novo.telefone[strcspn(novo.telefone, "\n")] = '\0';

    novo.pontos_fidelidade = 0;
    novo.materiais_reciclados_entregues = 0;

    clientes[total_clientes++] = novo;
    printf("\nCliente cadastrado com sucesso! ID: %d\n", novo.id);
    printf("Incentivo sustentável: Ao trazer materiais recicláveis, você ganha descontos!\n");
}

void cadastrar_fornecedor() {
    if (total_fornecedores >= MAX_PRODUTOS) {
        printf("\nLimite máximo de fornecedores atingido.\n");
        return;
    }

    Fornecedor novo;
    novo.id = total_fornecedores + 1;

    printf("\n--- CADASTRO DE FORNECEDOR ---\n");
    printf("Nome: ");
    fgets(novo.nome, MAX_NOME, stdin);
    novo.nome[strcspn(novo.nome, "\n")] = '\0';

    printf("Tipo (Cooperativa / Produtor Local): ");
    fgets(novo.tipo, MAX_NOME, stdin);
    novo.tipo[strcspn(novo.tipo, "\n")] = '\0';

    printf("Endereço: ");
    fgets(novo.endereco, MAX_NOME, stdin);
    novo.endereco[strcspn(novo.endereco, "\n")] = '\0';

    printf("Contato: ");
    fgets(novo.contato, MAX_NOME, stdin);
    novo.contato[strcspn(novo.contato, "\n")] = '\0';

    fornecedores[total_fornecedores++] = novo;
    printf("\nFornecedor cadastrado com sucesso! ID: %d\n", novo.id);
}

void listar_produtos() {
    printf("\n--- LISTA DE PRODUTOS ---\n");
    if (total_produtos == 0) {
        printf("Nenhum produto cadastrado.\n");
        return;
    }
    printf("ID | Nome                 | Preço (R$) | Estoque | Categoria   | Forn.\n");
    printf("---------------------------------------------------------------------\n");
    for (int i = 0; i < total_produtos; i++) {
        printf("%2d | %-20s | %9.2f | %7d | %-11s | %d\n",
               produtos[i].id, produtos[i].nome, produtos[i].preco,
               produtos[i].estoque, produtos[i].categoria, produtos[i].fornecedor_id);
    }
}

void listar_clientes() {
    printf("\n--- LISTA DE CLIENTES ---\n");
    if (total_clientes == 0) {
        printf("Nenhum cliente cadastrado.\n");
        return;
    }
    printf("ID | Nome                 | E-mail                    | Telefone      | Pontos | Mat. Entregues\n");
    printf("--------------------------------------------------------------------------------------------\n");
    for (int i = 0; i < total_clientes; i++) {
        printf("%2d | %-20s | %-25s | %-13s | %6d | %d\n",
               clientes[i].id, clientes[i].nome, clientes[i].email,
               clientes[i].telefone, clientes[i].pontos_fidelidade,
               clientes[i].materiais_reciclados_entregues);
    }
}

void listar_fornecedores() {
    printf("\n--- LISTA DE FORNECEDORES ---\n");
    if (total_fornecedores == 0) {
        printf("Nenhum fornecedor cadastrado.\n");
        return;
    }
    printf("ID | Nome                      | Tipo                     | Contato\n");
    printf("--------------------------------------------------------------------\n");
    for (int i = 0; i < total_fornecedores; i++) {
        printf("%2d | %-25s | %-25s | %s\n",
               fornecedores[i].id, fornecedores[i].nome,
               fornecedores[i].tipo, fornecedores[i].contato);
    }
}

/* ========================================================================
   FUNÇÕES DE BUSCA E CÁLCULO
   ======================================================================== */

int buscar_produto_por_id(int id) {
    for (int i = 0; i < total_produtos; i++)
        if (produtos[i].id == id) return i;
    return -1;
}

int buscar_cliente_por_id(int id) {
    for (int i = 0; i < total_clientes; i++)
        if (clientes[i].id == id) return i;
    return -1;
}

int contar_vendas_no_dia(int dia, int mes, int ano) {
    int cont = 0;
    for (int i = 0; i < total_pedidos; i++) {
        if (pedidos[i].dia == dia && pedidos[i].mes == mes && pedidos[i].ano == ano)
            cont++;
    }
    return cont;
}

float calcular_valor_item(ItemPedido item) {
    return item.quantidade * item.preco_unitario;
}

/* ========================================================================
   REGISTRO DE VENDA
   ======================================================================== */

void registrar_venda() {
    if (total_pedidos >= VENDAS_POR_DIA * DIAS_NO_ANO) {
        printf("\nLimite máximo de pedidos anual atingido.\n");
        return;
    }

    printf("\n--- REGISTRO DE VENDA (UPYCLE STORE) ---\n");

    int dia, mes, ano;
    do {
        printf("Data da venda (dia mês ano): ");
        if (scanf("%d %d %d", &dia, &mes, &ano) != 3) {
            printf("Entrada inválida! Digite três números separados por espaço.\n");
            limpar_buffer();
            continue;
        }
        limpar_buffer();
        if (!validar_data(dia, mes, ano)) {
            printf("Data inválida! Verifique o dia, mês e ano (ex: 15 6 2026).\n");
            continue;
        }
        break;
    } while (1);

    if (contar_vendas_no_dia(dia, mes, ano) >= VENDAS_POR_DIA) {
        printf("Limite de %d vendas por dia já atingido para esta data.\n", VENDAS_POR_DIA);
        return;
    }

    if (total_clientes == 0) {
        printf("Nenhum cliente cadastrado. Cadastre um cliente primeiro.\n");
        return;
    }

    listar_clientes();
    int cliente_id;
    do {
        printf("ID do cliente: ");
        if (scanf("%d", &cliente_id) != 1) {
            printf("Entrada inválida!\n");
            limpar_buffer();
            continue;
        }
        limpar_buffer();
        if (buscar_cliente_por_id(cliente_id) == -1) {
            printf("Cliente não encontrado!\n");
            continue;
        }
        break;
    } while (1);

    int idx_cliente = buscar_cliente_por_id(cliente_id);

    if (clientes[idx_cliente].materiais_reciclados_entregues > 0) {
        printf("Cliente possui %d materiais reciclados entregues. Desconto de 5%% aplicado!\n",
               clientes[idx_cliente].materiais_reciclados_entregues);
    }

    Pedido novo_pedido;
    novo_pedido.id = total_pedidos + 1;
    novo_pedido.cliente_id = cliente_id;
    novo_pedido.dia = dia;
    novo_pedido.mes = mes;
    novo_pedido.ano = ano;
    novo_pedido.qtd_itens = 0;
    novo_pedido.valor_total_pedido = 0.0;
    novo_pedido.taxa_transportadora = 0.0;
    novo_pedido.taxa_aplicada = 0;

    char continuar = 's';
    while (continuar == 's' || continuar == 'S') {
        if (novo_pedido.qtd_itens >= MAX_PRODUTOS) {
            printf("Limite de itens por pedido atingido.\n");
            break;
        }
        if (total_produtos == 0) {
            printf("Nenhum produto cadastrado. Cadastre um produto primeiro.\n");
            break;
        }

        listar_produtos();
        int prod_id;
        do {
            printf("ID do produto: ");
            if (scanf("%d", &prod_id) != 1) {
                printf("Entrada inválida!\n");
                limpar_buffer();
                continue;
            }
            limpar_buffer();
            if (buscar_produto_por_id(prod_id) == -1) {
                printf("Produto não encontrado!\n");
                continue;
            }
            break;
        } while (1);

        int idx_prod = buscar_produto_por_id(prod_id);
        printf("Quantidade disponível: %d\n", produtos[idx_prod].estoque);

        int qtd;
        do {
            printf("Quantidade desejada: ");
            if (scanf("%d", &qtd) != 1 || !validar_quantidade(qtd)) {
                printf("Quantidade inválida! Digite um valor maior que zero.\n");
                limpar_buffer();
                continue;
            }
            limpar_buffer();
            if (qtd > produtos[idx_prod].estoque) {
                printf("Estoque insuficiente! Disponível: %d\n", produtos[idx_prod].estoque);
                continue;
            }
            break;
        } while (1);

        ItemPedido novo_item;
        novo_item.produto_id = prod_id;
        novo_item.quantidade = qtd;
        novo_item.preco_unitario = produtos[idx_prod].preco;
        novo_item.valor_total_item = calcular_valor_item(novo_item);
        novo_item.numero_devolucoes = 0;   /* inicia zerado */

        novo_pedido.itens[novo_pedido.qtd_itens++] = novo_item;
        produtos[idx_prod].estoque -= qtd;
        novo_pedido.valor_total_pedido += novo_item.valor_total_item;

        printf("Item adicionado! Subtotal: R$ %.2f\n", novo_pedido.valor_total_pedido);

        do {
            printf("Adicionar mais itens? (s/n): ");
            scanf(" %c", &continuar);
            limpar_buffer();
            if (continuar != 's' && continuar != 'S' && continuar != 'n' && continuar != 'N')
                printf("Resposta inválida! Digite 's' ou 'n'.\n");
            else break;
        } while (1);
    }

    /* Desconto de 5% se cliente tiver materiais reciclados entregues */
    float desconto = 0.0;
    if (clientes[idx_cliente].materiais_reciclados_entregues > 0) {
        desconto = novo_pedido.valor_total_pedido * 0.05;
        novo_pedido.valor_total_pedido -= desconto;
        printf("Desconto de 5%% aplicado: -R$ %.2f\n", desconto);
        clientes[idx_cliente].materiais_reciclados_entregues = 0;
    }

    pedidos[total_pedidos++] = novo_pedido;

    printf("\n===========================================================\n");
    printf("VENDA REGISTRADA COM SUCESSO!\n");
    printf("Pedido Nº: %d\n", novo_pedido.id);
    printf("Cliente: %s\n", clientes[idx_cliente].nome);
    printf("Itens: %d\n", novo_pedido.qtd_itens);
    printf("Valor Total: R$ %.2f\n", novo_pedido.valor_total_pedido);
    if (desconto > 0) printf("Desconto Sustentável: -R$ %.2f\n", desconto);
    printf("===========================================================\n");
}

/* ========================================================================
   REGISTRO DE DEVOLUÇÃO – com a nova lógica de taxa por item (segunda devolução do mesmo item)
   ======================================================================== */

void registrar_devolucao() {
    printf("\n--- REGISTRO DE DEVOLUÇÃO ---\n");

    if (total_pedidos == 0) {
        printf("Nenhum pedido registrado.\n");
        return;
    }
    if (total_clientes == 0) {
        printf("Nenhum cliente cadastrado.\n");
        return;
    }

    listar_clientes();

    int cliente_id;
    do {
        printf("Digite o ID do cliente: ");
        if (scanf("%d", &cliente_id) != 1) {
            printf("Entrada inválida!\n");
            limpar_buffer();
            continue;
        }
        limpar_buffer();
        if (buscar_cliente_por_id(cliente_id) == -1) {
            printf("Cliente não encontrado!\n");
            continue;
        }
        break;
    } while (1);

    int idx_cliente = buscar_cliente_por_id(cliente_id);

    printf("\n--- PEDIDOS DO CLIENTE: %s ---\n", clientes[idx_cliente].nome);
    printf("ID Pedido | Data       | Valor Total (R$) | Itens\n");
    printf("------------------------------------------------------\n");

    int pedidos_cliente[100], qtd = 0;
    for (int i = 0; i < total_pedidos; i++) {
        if (pedidos[i].cliente_id == cliente_id) {
            printf("   %5d   | %02d/%02d/%04d | %15.2f | %5d\n",
                   pedidos[i].id, pedidos[i].dia, pedidos[i].mes, pedidos[i].ano,
                   pedidos[i].valor_total_pedido, pedidos[i].qtd_itens);
            pedidos_cliente[qtd++] = i;
        }
    }

    if (qtd == 0) {
        printf("Este cliente não possui pedidos.\n");
        return;
    }

    int pedido_id;
    do {
        printf("\nDigite o ID do pedido para devolução: ");
        if (scanf("%d", &pedido_id) != 1) {
            printf("Entrada inválida!\n");
            limpar_buffer();
            continue;
        }
        limpar_buffer();
        int encontrado = 0;
        for (int i = 0; i < total_pedidos; i++) {
            if (pedidos[i].id == pedido_id && pedidos[i].cliente_id == cliente_id) {
                encontrado = 1;
                break;
            }
        }
        if (!encontrado) {
            printf("Pedido não encontrado para este cliente!\n");
            continue;
        }
        break;
    } while (1);

    int idx_pedido = -1;
    for (int i = 0; i < total_pedidos; i++) {
        if (pedidos[i].id == pedido_id && pedidos[i].cliente_id == cliente_id) {
            idx_pedido = i;
            break;
        }
    }

    Pedido *p = &pedidos[idx_pedido];

    if (p->qtd_itens == 0) {
        printf("Este pedido não possui itens para devolver.\n");
        return;
    }

    printf("\n--- ITENS DO PEDIDO %d ---\n", p->id);
    printf("ID Produto | Nome                 | Quantidade | Preço Unit. | Devoluções (histórico)\n");
    printf("----------------------------------------------------------------------------------\n");

    for (int i = 0; i < p->qtd_itens; i++) {
        int idx_prod = buscar_produto_por_id(p->itens[i].produto_id);
        char nome_prod[MAX_NOME];
        if (idx_prod != -1) strcpy(nome_prod, produtos[idx_prod].nome);
        else strcpy(nome_prod, "Produto removido");
        printf("   %5d   | %-20s | %10d | %11.2f | %d\n",
               p->itens[i].produto_id, nome_prod,
               p->itens[i].quantidade, p->itens[i].preco_unitario,
               p->itens[i].numero_devolucoes);
    }

    int prod_id;
    do {
        printf("\nDigite o ID do produto que será devolvido: ");
        if (scanf("%d", &prod_id) != 1) {
            printf("Entrada inválida!\n");
            limpar_buffer();
            continue;
        }
        limpar_buffer();
        int encontrado = 0;
        for (int i = 0; i < p->qtd_itens; i++) {
            if (p->itens[i].produto_id == prod_id) {
                encontrado = 1;
                break;
            }
        }
        if (!encontrado) {
            printf("Produto não encontrado neste pedido!\n");
            continue;
        }
        break;
    } while (1);

    int idx_item = -1;
    for (int i = 0; i < p->qtd_itens; i++) {
        if (p->itens[i].produto_id == prod_id) {
            idx_item = i;
            break;
        }
    }

    /* Incrementa histórico de devoluções do item */
    p->itens[idx_item].numero_devolucoes++;

    /* Repõe o estoque do produto (quantidade devolvida) */
    int idx_prod = buscar_produto_por_id(prod_id);
    if (idx_prod != -1) {
        produtos[idx_prod].estoque += p->itens[idx_item].quantidade;
        printf("Estoque do produto '%s' reposto em %d unidades.\n",
               produtos[idx_prod].nome, p->itens[idx_item].quantidade);
    }

    /* Verifica se este item já foi devolvido pelo menos duas vezes (agora conta 2) */
    if (p->itens[idx_item].numero_devolucoes >= 2 && p->taxa_aplicada == 0) {
        p->taxa_transportadora = 20.00;
        p->taxa_aplicada = 1;
        p->valor_total_pedido += 20.00;

        printf("\n[TAXA APLICADA] R$ 20,00 foi adicionado ao pedido devido à segunda devolução do mesmo item.\n");
        printf("Novo valor total do pedido: R$ %.2f\n", p->valor_total_pedido);
    } else if (p->itens[idx_item].numero_devolucoes >= 2 && p->taxa_aplicada == 1) {
        printf("\nA taxa de R$ 20,00 já foi aplicada a este pedido. Nenhuma taxa adicional.\n");
    } else {
        printf("\nPrimeira devolução deste item. Nenhuma taxa aplicada.\n");
    }

    printf("\n===========================================================\n");
    printf("DEVOLUÇÃO FINALIZADA COM SUCESSO!\n");
    printf("Cliente: %s\n", clientes[idx_cliente].nome);
    printf("Pedido Nº: %d\n", p->id);
    if (idx_prod != -1) printf("Produto devolvido: %s\n", produtos[idx_prod].nome);
    printf("Total de devoluções deste item: %d\n", p->itens[idx_item].numero_devolucoes);
    printf("Valor total atual do pedido: R$ %.2f\n", p->valor_total_pedido);
    printf("===========================================================\n");
}

/* ========================================================================
   RELATÓRIOS
   ======================================================================== */

void relatorio_diario() {
    printf("\n===========================================================\n");
    printf("            RELATÓRIO DIÁRIO - UPYCLE STORE\n");
    printf("===========================================================\n");

    if (total_pedidos == 0) {
        printf("Nenhum pedido registrado.\n");
        return;
    }

    int dia, mes, ano;
    do {
        printf("Informe a data (dia mês ano): ");
        if (scanf("%d %d %d", &dia, &mes, &ano) != 3) {
            printf("Entrada inválida! Digite três números.\n");
            limpar_buffer();
            continue;
        }
        limpar_buffer();
        if (!validar_data(dia, mes, ano)) {
            printf("Data inválida! Tente novamente.\n");
            continue;
        }
        break;
    } while (1);

    printf("\n--- PEDIDOS DO DIA %02d/%02d/%04d ---\n", dia, mes, ano);
    printf("ID Pedido | Cliente ID | Valor Total (R$)\n");
    printf("----------------------------------------\n");

    float total_dia = 0.0;
    int encontrou = 0;
    for (int i = 0; i < total_pedidos; i++) {
        if (pedidos[i].dia == dia && pedidos[i].mes == mes && pedidos[i].ano == ano) {
            printf("   %5d   |    %4d    |     %8.2f\n",
                   pedidos[i].id, pedidos[i].cliente_id, pedidos[i].valor_total_pedido);
            total_dia += pedidos[i].valor_total_pedido;
            encontrou = 1;
        }
    }

    if (!encontrou) {
        printf("Nenhum pedido encontrado para esta data.\n");
    } else {
        printf("----------------------------------------\n");
        printf("TOTAL DE VENDAS DO DIA: R$ %.2f\n", total_dia);
    }
    printf("===========================================================\n");
}

void relatorio_mensal() {
    printf("\n===========================================================\n");
    printf("            RELATÓRIO MENSAL - UPYCLE STORE\n");
    printf("===========================================================\n");

    if (total_pedidos == 0) {
        printf("Nenhum pedido registrado.\n");
        return;
    }

    int ano;
    printf("Informe o ano para o relatório: ");
    scanf("%d", &ano);
    limpar_buffer();

    float total_mensal[MESES_NO_ANO] = {0};

    for (int i = 0; i < total_pedidos; i++) {
        if (pedidos[i].ano == ano) {
            total_mensal[pedidos[i].mes - 1] += pedidos[i].valor_total_pedido;
        }
    }

    printf("\n--- RESUMO MENSAL DO ANO %d ---\n", ano);
    printf("Mês           | Total Vendido (R$)\n");
    printf("--------------------------------\n");

    float total_ano = 0.0;
    const char *nomes_meses[] = {
        "Janeiro", "Fevereiro", "Março", "Abril", "Maio", "Junho",
        "Julho", "Agosto", "Setembro", "Outubro", "Novembro", "Dezembro"
    };

    for (int i = 0; i < MESES_NO_ANO; i++) {
        printf("%-12s | %15.2f\n", nomes_meses[i], total_mensal[i]);
        total_ano += total_mensal[i];
    }

    printf("--------------------------------\n");
    printf("TOTAL CONSOLIDADO DO ANO: R$ %.2f\n", total_ano);
    printf("===========================================================\n");
}

void relatorio_anual() {
    printf("\n===========================================================\n");
    printf("            RELATÓRIO ANUAL - UPYCLE STORE\n");
    printf("===========================================================\n");

    if (total_pedidos == 0) {
        printf("Nenhum pedido registrado.\n");
        return;
    }

    int ano;
    printf("Informe o ano para o relatório: ");
    scanf("%d", &ano);
    limpar_buffer();

    float total_mensal[MESES_NO_ANO] = {0};
    for (int i = 0; i < total_pedidos; i++) {
        if (pedidos[i].ano == ano) {
            total_mensal[pedidos[i].mes - 1] += pedidos[i].valor_total_pedido;
        }
    }

    float total_ano = 0.0;
    for (int i = 0; i < MESES_NO_ANO; i++) total_ano += total_mensal[i];

    printf("\nTOTAL DE VENDAS REALIZADAS NO ANO %d: R$ %.2f\n", ano, total_ano);

    float valores[MESES_NO_ANO];
    int meses[MESES_NO_ANO];
    int qtd = 0;

    for (int i = 0; i < MESES_NO_ANO; i++) {
        if (total_mensal[i] > 0) {
            valores[qtd] = total_mensal[i];
            meses[qtd] = i;
            qtd++;
        }
    }

    if (qtd == 0) {
        printf("Nenhuma venda registrada no ano %d.\n", ano);
        return;
    }

    ordenar_por_faturamento(valores, meses, qtd);

    const char *nomes_meses[] = {
        "Janeiro", "Fevereiro", "Março", "Abril", "Maio", "Junho",
        "Julho", "Agosto", "Setembro", "Outubro", "Novembro", "Dezembro"
    };

    printf("\n--- MESES EM ORDEM DECRESCENTE DE FATURAMENTO ---\n");
    printf("Posição | Mês           | Faturamento (R$)\n");
    printf("-----------------------------------------------\n");
    for (int i = 0; i < qtd; i++) {
        printf("  %2d    | %-12s | %15.2f\n",
               i + 1, nomes_meses[meses[i]], valores[i]);
    }
    printf("===========================================================\n");
}

/* ========================================================================
   ALGORITMO DE ORDENAÇÃO (Bubble Sort – decrescente)
   ======================================================================== */

void ordenar_por_faturamento(float valores[], int meses[], int tamanho) {
    for (int i = 0; i < tamanho - 1; i++) {
        for (int j = 0; j < tamanho - i - 1; j++) {
            if (valores[j] < valores[j + 1]) {
                float temp_valor = valores[j];
                valores[j] = valores[j + 1];
                valores[j + 1] = temp_valor;
                int temp_mes = meses[j];
                meses[j] = meses[j + 1];
                meses[j + 1] = temp_mes;
            }
        }
    }
}