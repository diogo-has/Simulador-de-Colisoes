#include "raylib.h"
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <math.h>
#include <locale.h>

typedef struct {
    double x;
    double y;
    double vx;
    double vy;
    int r;
    int m;
    Color cor;
} Particula;

Particula*** combinatoria(Particula* vet, int tam, int* npares);

int main() {
    srand(time(0));
    setlocale(LC_ALL,"");

    int largura_tela, altura_tela, npares, nparts, i;
    Particula*** pares;
    double cr;

    printf("Insira largura e altura da tela:\n");
    scanf("%d %d", &largura_tela, &altura_tela);
    printf("Insira a quantidade de partículas:\n");
    scanf("%d", &nparts);
    printf("Insira o coeficiente de restituição de velocidade:\n");
    scanf("%lf", &cr);

    InitWindow(largura_tela, altura_tela, "Simulador de Colisoes");

    SetTargetFPS(60);

    Particula particulas[nparts];
    for (i = 0; i < nparts; i++) {
        particulas[i].r = 30 + rand() % 11;
        particulas[i].m = particulas[i].r;
        particulas[i].x = (rand() % (largura_tela - particulas[i].r*2 - 1)) + particulas[i].r;
        particulas[i].y = (rand() % (altura_tela - particulas[i].r*2 - 1)) + particulas[i].r;
        particulas[i].vx = rand() % 11 - 5;
        particulas[i].vy = rand() % 11 - 5;
        particulas[i].cor = (Color){rand() % 100, rand() % 100, rand() % 100, 255};
    }

    pares = combinatoria(particulas, nparts, &npares);

    while (!WindowShouldClose()) {

        for (i = 0; i < nparts; i++) {
            particulas[i].x += particulas[i].vx;
            particulas[i].y += particulas[i].vy;
        }

        for (i = 0; i < npares; i++) {
            Particula* p1 = pares[i][0];
            Particula* p2 = pares[i][1];

            double dist = hypot(p1->x - p2->x, p1->y - p2->y);

            if (dist <= p1->r + p2->r) {
                double overlap = (p1->r + p2->r) - dist;
                double dx = p2->x - p1->x;
                double dy = p2->y - p1->y;

                p1->x -= (overlap * (dx / dist)) / 2.0;
                p1->y -= (overlap * (dy / dist)) / 2.0;
                p2->x += (overlap * (dx / dist)) / 2.0;
                p2->y += (overlap * (dy / dist)) / 2.0;

                if (cr == 0.0) {
                    double vcm_x = (p1->m * p1->vx + p2->m * p2->vx) / (p1->m + p2->m);
                    double vcm_y = (p1->m * p1->vy + p2->m * p2->vy) / (p1->m + p2->m);
                    p1->vx = vcm_x;
                    p1->vy = vcm_y;
                    p2->vx = vcm_x;
                    p2->vy = vcm_y;
                } else {
                    double ang_col = atan2(dy, dx);
                    double v1c = p1->vx * cos(ang_col) + p1->vy * sin(ang_col);
                    double v2c = p2->vx * cos(ang_col) + p2->vy * sin(ang_col);
                    double v1t = p1->vx * -sin(ang_col) + p1->vy * cos(ang_col);
                    double v2t = p2->vx * -sin(ang_col) + p2->vy * cos(ang_col);

                    double vcm = (p1->m * v1c + p2->m * v2c) / (p1->m + p2->m);
                    double v1fc = (1 + cr) * vcm - cr * v1c;
                    double v2fc = (1 + cr) * vcm - cr * v2c;

                    p1->vx = v1fc * cos(ang_col) + v1t * -sin(ang_col);
                    p1->vy = v1fc * sin(ang_col) + v1t * cos(ang_col);
                    p2->vx = v2fc * cos(ang_col) + v2t * -sin(ang_col);
                    p2->vy = v2fc * sin(ang_col) + v2t * cos(ang_col);
                }
            }
        }

        for (i = 0; i < nparts; i++) {
            if (particulas[i].x + particulas[i].r > largura_tela) {
                particulas[i].x = largura_tela - particulas[i].r;
                particulas[i].vx *= -1;
            }
            if (particulas[i].y + particulas[i].r > altura_tela) {
                particulas[i].y = altura_tela - particulas[i].r;
                particulas[i].vy *= -1;
            }
            if (particulas[i].x - particulas[i].r < 0) {
                particulas[i].x = particulas[i].r;
                particulas[i].vx *= -1;
            }
            if (particulas[i].y - particulas[i].r < 0) {
                particulas[i].y = particulas[i].r;
                particulas[i].vy *= -1;
            }
        }

        BeginDrawing();
            ClearBackground(RAYWHITE);
            for (i = 0; i < nparts; i++)
                DrawCircle(particulas[i].x, particulas[i].y, particulas[i].r, particulas[i].cor);

        EndDrawing();
    }

    CloseWindow();

    for (i = 0; i < npares; i++)
        free(pares[i]);
    free(pares);
    return 0;
}

Particula*** combinatoria(Particula* vet, int tam, int* npares) {
    int num_combinacoes = tam * (tam - 1) / 2;
    Particula*** resultado = (Particula***)malloc(num_combinacoes * sizeof(Particula**));

    int k = 0;
    for (int i = 0; i < tam - 1; i++) {
        for (int j = i + 1; j < tam; j++) {
            resultado[k] = (Particula**) malloc(2 * sizeof(Particula*));
            resultado[k][0] = &vet[i];
            resultado[k][1] = &vet[j];
            k++;
        }
    }

    *npares = num_combinacoes;
    return resultado;
}
