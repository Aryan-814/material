// Name: Aryan
// Roll Number: 2401CS48

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct {
    char lhs[10];
    char rhs[10][10];
    int rhs_len;
} Prod;

typedef struct {
    char name[10];
    Prod prods[20];
    int num_prods;
    char nts[10][10];
    int num_nts;
    char terms[10][10];
    int num_terms;
    char start_nt[10];
} Grammar;

int table[10][10][10];
int table_counts[10][10];

int is_nt(Grammar *g, const char *sym) {
    for (int i = 0; i < g->num_nts; i++) {
        if (strcmp(g->nts[i], sym) == 0) return 1;
    }
    return 0;
}

int is_term(Grammar *g, const char *sym) {
    for (int i = 0; i < g->num_terms; i++) {
        if (strcmp(g->terms[i], sym) == 0) return 1;
    }
    return 0;
}

int add_to_set(char set[20][10], int *count, const char *sym) {
    for (int i = 0; i < *count; i++) {
        if (strcmp(set[i], sym) == 0) return 0;
    }
    strcpy(set[*count], sym);
    (*count)++;
    return 1;
}

void compute_first_seq(Grammar *g, char rhs[10][10], int rhs_len, char first_out[20][10], int *first_count, char first_sets[10][20][10], int first_counts[10]) {
    *first_count = 0;
    if (rhs_len == 0 || strcmp(rhs[0], "e") == 0) {
        add_to_set(first_out, first_count, "e");
        return;
    }
    for (int i = 0; i < rhs_len; i++) {
        if (is_term(g, rhs[i])) {
            add_to_set(first_out, first_count, rhs[i]);
            break;
        } else {
            int nt_idx = -1;
            for (int j = 0; j < g->num_nts; j++) {
                if (strcmp(g->nts[j], rhs[i]) == 0) {
                    nt_idx = j;
                    break;
                }
            }
            if (nt_idx != -1) {
                int has_e = 0;
                for (int k = 0; k < first_counts[nt_idx]; k++) {
                    if (strcmp(first_sets[nt_idx][k], "e") == 0) {
                        has_e = 1;
                    } else {
                        add_to_set(first_out, first_count, first_sets[nt_idx][k]);
                    }
                }
                if (!has_e) break;
                if (i == rhs_len - 1) {
                    add_to_set(first_out, first_count, "e");
                }
            }
        }
    }
}

void compute_first(Grammar *g, char first_sets[10][20][10], int first_counts[10]) {
    for (int i = 0; i < g->num_nts; i++) {
        first_counts[i] = 0;
    }
    int changed = 1;
    while (changed) {
        changed = 0;
        for (int i = 0; i < g->num_prods; i++) {
            int nt_idx = -1;
            for (int j = 0; j < g->num_nts; j++) {
                if (strcmp(g->nts[j], g->prods[i].lhs) == 0) {
                    nt_idx = j;
                    break;
                }
            }
            if (nt_idx == -1) continue;

            char seq_first[20][10];
            int seq_count = 0;
            compute_first_seq(g, g->prods[i].rhs, g->prods[i].rhs_len, seq_first, &seq_count, first_sets, first_counts);

            for (int k = 0; k < seq_count; k++) {
                if (add_to_set(first_sets[nt_idx], &first_counts[nt_idx], seq_first[k])) {
                    changed = 1;
                }
            }
        }
    }
}

void compute_follow(Grammar *g, char first_sets[10][20][10], int first_counts[10], char follow_sets[10][20][10], int follow_counts[10]) {
    for (int i = 0; i < g->num_nts; i++) {
        follow_counts[i] = 0;
    }
    for (int i = 0; i < g->num_nts; i++) {
        if (strcmp(g->nts[i], g->start_nt) == 0) {
            add_to_set(follow_sets[i], &follow_counts[i], "$");
            break;
        }
    }

    int changed = 1;
    while (changed) {
        changed = 0;
        for (int p = 0; p < g->num_prods; p++) {
            int head_idx = -1;
            for (int j = 0; j < g->num_nts; j++) {
                if (strcmp(g->nts[j], g->prods[p].lhs) == 0) {
                    head_idx = j;
                    break;
                }
            }
            for (int i = 0; i < g->prods[p].rhs_len; i++) {
                int nt_idx = -1;
                for (int j = 0; j < g->num_nts; j++) {
                    if (strcmp(g->nts[j], g->prods[p].rhs[i]) == 0) {
                        nt_idx = j;
                        break;
                    }
                }
                if (nt_idx != -1) {
                    char seq_first[20][10];
                    int seq_count = 0;
                    compute_first_seq(g, &g->prods[p].rhs[i + 1], g->prods[p].rhs_len - (i + 1), seq_first, &seq_count, first_sets, first_counts);

                    int has_e = 0;
                    for (int k = 0; k < seq_count; k++) {
                        if (strcmp(seq_first[k], "e") == 0) {
                            has_e = 1;
                        } else {
                            if (add_to_set(follow_sets[nt_idx], &follow_counts[nt_idx], seq_first[k])) {
                                changed = 1;
                            }
                        }
                    }
                    if (has_e || (i + 1 == g->prods[p].rhs_len)) {
                        for (int k = 0; k < follow_counts[head_idx]; k++) {
                            if (add_to_set(follow_sets[nt_idx], &follow_counts[nt_idx], follow_sets[head_idx][k])) {
                                changed = 1;
                            }
                        }
                    }
                }
            }
        }
    }
}

int build_ll1_table(Grammar *g, char first_sets[10][20][10], int first_counts[10], char follow_sets[10][20][10], int follow_counts[10]) {
    for (int i = 0; i < g->num_nts; i++) {
        for (int j = 0; j < g->num_terms; j++) {
            table_counts[i][j] = 0;
        }
    }

    for (int i = 0; i < g->num_prods; i++) {
        int nt_idx = -1;
        for (int j = 0; j < g->num_nts; j++) {
            if (strcmp(g->nts[j], g->prods[i].lhs) == 0) {
                nt_idx = j;
                break;
            }
        }

        char seq_first[20][10];
        int seq_count = 0;
        compute_first_seq(g, g->prods[i].rhs, g->prods[i].rhs_len, seq_first, &seq_count, first_sets, first_counts);

        int has_e = 0;
        for (int k = 0; k < seq_count; k++) {
            if (strcmp(seq_first[k], "e") == 0) {
                has_e = 1;
            } else {
                int term_idx = -1;
                for (int t = 0; t < g->num_terms; t++) {
                    if (strcmp(g->terms[t], seq_first[k]) == 0) {
                        term_idx = t;
                        break;
                    }
                }
                if (term_idx != -1) {
                    table[nt_idx][term_idx][table_counts[nt_idx][term_idx]++] = i;
                }
            }
        }

        if (has_e) {
            for (int k = 0; k < follow_counts[nt_idx]; k++) {
                int term_idx = -1;
                for (int t = 0; t < g->num_terms; t++) {
                    if (strcmp(g->terms[t], follow_sets[nt_idx][k]) == 0) {
                        term_idx = t;
                        break;
                    }
                }
                if (term_idx != -1) {
                    table[nt_idx][term_idx][table_counts[nt_idx][term_idx]++] = i;
                }
            }
        }
    }

    int has_conflict = 0;
    for (int i = 0; i < g->num_nts; i++) {
        for (int j = 0; j < g->num_terms; j++) {
            if (table_counts[i][j] > 1) {
                has_conflict = 1;
                printf("LL(1) conflict at M[%s, %s]:\n", g->nts[i], g->terms[j]);
                for (int c = 0; c < table_counts[i][j]; c++) {
                    int p_idx = table[i][j][c];
                    printf("  %s ->", g->prods[p_idx].lhs);
                    for (int r = 0; r < g->prods[p_idx].rhs_len; r++) {
                        printf(" %s", g->prods[p_idx].rhs[r]);
                    }
                    printf("\n");
                }
            }
        }
    }

    if (has_conflict) {
        printf("Grammar %s is not LL(1).\n", g->name);
        return 0;
    }
    return 1;
}

void print_prod(Prod *p) {
    printf("%s ->", p->lhs);
    for (int i = 0; i < p->rhs_len; i++) {
        printf(" %s", p->rhs[i]);
    }
    printf("\n");
}

int parse(Grammar *g, char tokens[][10], int num_tokens) {
    char stack[500][10];
    int top = 0;
    strcpy(stack[0], "$");
    top++;
    strcpy(stack[1], g->start_nt);

    int token_idx = 0;
    while (top >= 0) {
        char current_symbol[10];
        strcpy(current_symbol, stack[top]);
        char current_token[10];
        strcpy(current_token, tokens[token_idx]);

        if (is_term(g, current_symbol)) {
            if (strcmp(current_symbol, current_token) == 0) {
                top--;
                token_idx++;
                if (strcmp(current_symbol, "$") == 0) {
                    printf("ACCEPT\n");
                    return 1;
                }
            } else {
                printf("REJECT\n");
                return 0;
            }
        } else if (is_nt(g, current_symbol)) {
            int nt_idx = -1;
            for (int i = 0; i < g->num_nts; i++) {
                if (strcmp(g->nts[i], current_symbol) == 0) {
                    nt_idx = i;
                    break;
                }
            }
            int term_idx = -1;
            for (int j = 0; j < g->num_terms; j++) {
                if (strcmp(g->terms[j], current_token) == 0) {
                    term_idx = j;
                    break;
                }
            }
            if (nt_idx == -1 || term_idx == -1 || table_counts[nt_idx][term_idx] == 0) {
                printf("REJECT\n");
                return 0;
            }
            int prod_idx = table[nt_idx][term_idx][0];
            print_prod(&g->prods[prod_idx]);
            top--;
            if (!(g->prods[prod_idx].rhs_len == 1 && strcmp(g->prods[prod_idx].rhs[0], "e") == 0)) {
                for (int r = g->prods[prod_idx].rhs_len - 1; r >= 0; r--) {
                    top++;
                    strcpy(stack[top], g->prods[prod_idx].rhs[r]);
                }
            }
        } else {
            printf("REJECT\n");
            return 0;
        }
    }
    printf("REJECT\n");
    return 0;
}

int tokenize(const char *input, char tokens[500][10]) {
    int num_tokens = 0;
    int i = 0;
    int len = strlen(input);
    while (i < len) {
        while (i < len && isspace((unsigned char)input[i])) i++;
        if (i >= len) break;

        if (input[i] == '+' || input[i] == '*' || input[i] == '(' || input[i] == ')') {
            tokens[num_tokens][0] = input[i];
            tokens[num_tokens][1] = '\0';
            num_tokens++;
            i++;
        } else if (isalnum((unsigned char)input[i]) || input[i] == '_') {
            while (i < len && (isalnum((unsigned char)input[i]) || input[i] == '_')) i++;
            strcpy(tokens[num_tokens], "id");
            num_tokens++;
        } else {
            i++;
        }
    }
    strcpy(tokens[num_tokens], "$");
    num_tokens++;
    return num_tokens;
}

int main(int argc, char *argv[]) {
    Grammar g1;
    strcpy(g1.name, "G1");
    g1.num_nts = 1;
    strcpy(g1.nts[0], "E");
    g1.num_terms = 6;
    strcpy(g1.terms[0], "id");
    strcpy(g1.terms[1], "+");
    strcpy(g1.terms[2], "*");
    strcpy(g1.terms[3], "(");
    strcpy(g1.terms[4], ")");
    strcpy(g1.terms[5], "$");
    strcpy(g1.start_nt, "E");
    g1.num_prods = 4;

    strcpy(g1.prods[0].lhs, "E");
    strcpy(g1.prods[0].rhs[0], "E");
    strcpy(g1.prods[0].rhs[1], "+");
    strcpy(g1.prods[0].rhs[2], "E");
    g1.prods[0].rhs_len = 3;

    strcpy(g1.prods[1].lhs, "E");
    strcpy(g1.prods[1].rhs[0], "E");
    strcpy(g1.prods[1].rhs[1], "*");
    strcpy(g1.prods[1].rhs[2], "E");
    g1.prods[1].rhs_len = 3;

    strcpy(g1.prods[2].lhs, "E");
    strcpy(g1.prods[2].rhs[0], "(");
    strcpy(g1.prods[2].rhs[1], "E");
    strcpy(g1.prods[2].rhs[2], ")");
    g1.prods[2].rhs_len = 3;

    strcpy(g1.prods[3].lhs, "E");
    strcpy(g1.prods[3].rhs[0], "id");
    g1.prods[3].rhs_len = 1;

    Grammar g2;
    strcpy(g2.name, "G2");
    g2.num_nts = 5;
    strcpy(g2.nts[0], "E");
    strcpy(g2.nts[1], "E'");
    strcpy(g2.nts[2], "T");
    strcpy(g2.nts[3], "T'");
    strcpy(g2.nts[4], "F");

    g2.num_terms = 6;
    strcpy(g2.terms[0], "id");
    strcpy(g2.terms[1], "+");
    strcpy(g2.terms[2], "*");
    strcpy(g2.terms[3], "(");
    strcpy(g2.terms[4], ")");
    strcpy(g2.terms[5], "$");

    strcpy(g2.start_nt, "E");
    g2.num_prods = 8;

    strcpy(g2.prods[0].lhs, "E");
    strcpy(g2.prods[0].rhs[0], "T");
    strcpy(g2.prods[0].rhs[1], "E'");
    g2.prods[0].rhs_len = 2;

    strcpy(g2.prods[1].lhs, "E'");
    strcpy(g2.prods[1].rhs[0], "+");
    strcpy(g2.prods[1].rhs[1], "T");
    strcpy(g2.prods[1].rhs[2], "E'");
    g2.prods[1].rhs_len = 3;

    strcpy(g2.prods[2].lhs, "E'");
    strcpy(g2.prods[2].rhs[0], "e");
    g2.prods[2].rhs_len = 1;

    strcpy(g2.prods[3].lhs, "T");
    strcpy(g2.prods[3].rhs[0], "F");
    strcpy(g2.prods[3].rhs[1], "T'");
    g2.prods[3].rhs_len = 2;

    strcpy(g2.prods[4].lhs, "T'");
    strcpy(g2.prods[4].rhs[0], "*");
    strcpy(g2.prods[4].rhs[1], "F");
    strcpy(g2.prods[4].rhs[2], "T'");
    g2.prods[4].rhs_len = 3;

    strcpy(g2.prods[5].lhs, "T'");
    strcpy(g2.prods[5].rhs[0], "e");
    g2.prods[5].rhs_len = 1;

    strcpy(g2.prods[6].lhs, "F");
    strcpy(g2.prods[6].rhs[0], "(");
    strcpy(g2.prods[6].rhs[1], "E");
    strcpy(g2.prods[6].rhs[2], ")");
    g2.prods[6].rhs_len = 3;

    strcpy(g2.prods[7].lhs, "F");
    strcpy(g2.prods[7].rhs[0], "id");
    g2.prods[7].rhs_len = 1;

    char g1_first[10][20][10], g1_follow[10][20][10];
    int g1_fc[10], g1_flc[10];
    compute_first(&g1, g1_first, g1_fc);
    compute_follow(&g1, g1_first, g1_fc, g1_follow, g1_flc);
    build_ll1_table(&g1, g1_first, g1_fc, g1_follow, g1_flc);

    FILE *fp = NULL;
    char buffer[1024] = "";
    if (argc > 1) {
        fp = fopen(argv[1], "r");
    }
    if (fp) {
        if (fgets(buffer, sizeof(buffer), fp) != NULL) {
            size_t len = strlen(buffer);
            if (len > 0 && buffer[len - 1] == '\n') buffer[len - 1] = '\0';
        }
        fclose(fp);
    } else {
        if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
            size_t len = strlen(buffer);
            if (len > 0 && buffer[len - 1] == '\n') buffer[len - 1] = '\0';
        }
    }

    char tokens[500][10];
    int num_tokens = tokenize(buffer, tokens);

    char g2_first[10][20][10], g2_follow[10][20][10];
    int g2_fc[10], g2_flc[10];
    compute_first(&g2, g2_first, g2_fc);
    compute_follow(&g2, g2_first, g2_fc, g2_follow, g2_flc);

    if (build_ll1_table(&g2, g2_first, g2_fc, g2_follow, g2_flc)) {
        printf("ll1: parsing with G2 using LL(1) table...\n");
        parse(&g2, tokens, num_tokens);
    }

    return 0;
}
