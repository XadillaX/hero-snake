#include "Rank.h"

Rank::Rank() {
    FILE *fp = fopen("data\\rank.txt", "r");
    for (int i = 0; i < 10; i++) {
        sprintf(R[i].Id, "%d", i + 1);
        strcpy(R[i].Score, "0");
        strcpy(R[i].Level, "0");
        fscanf(fp, "%s%s%s", &R[i].Score, &R[i].Level, &R[i].Name);
    }
    fclose(fp);
}

void Rank::SaveRank() {
    FILE *fp = fopen("data\\rank.txt", "w");
    for (int i = 0; i < 10; i++) {
        fprintf(fp, "%s %s %s\n", R[i].Score, R[i].Level, R[i].Name);
    }
    fclose(fp);
}

Rank::~Rank() {
}
