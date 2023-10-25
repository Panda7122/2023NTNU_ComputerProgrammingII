#include "oop.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
void print_potato(const struct Potato *this) {
    if (this == NULL) return;
    printf("Potato:\n");
    printf("  weight: %d\n", this->weight);
    printf("  produced by: %s\n", this->producer_name);
    return;
}
void print_magical_potato(const struct Potato *this) {
    if (this == NULL) return;
    struct MagicalPotato *tmp;
    tmp = malloc(sizeof(struct MagicalPotato));
    memcpy(tmp, this, sizeof(struct MagicalPotato));
    printf("Magical Potato:\n");
    printf("  weight: %d\n", tmp->base.weight);
    printf("  produced by: %s\n", tmp->base.producer_name);
    printf("  magical effect: %s\n", MagicalEffectToString[tmp->effect]);
    printf("  magic level: %d\n", tmp->effect_level);
    free(tmp);
    return;
}
void potato_dtor(struct Potato **obj) {
    if (*obj == NULL) return;
    free(*obj);
    *obj = NULL;
    return;
};
void magical_potato_dtor(struct Potato **obj) {
    if (*obj == NULL) return;
    free((struct MagicalPotato *)(*obj));
    *obj = NULL;
    return;
};

void print_potato_ball(const struct PotatoBall *this) {
    if (this == NULL) return;
    printf("Potato Ball:\n");
    printf("  weight: %d\n", this->weight);
    printf("  produced by: %s\n", this->producer_name);
    return;
}
void print_magical_potato_ball(const struct PotatoBall *this) {
    if (this == NULL) return;
    struct MagicalPotatoBall *tmp;
    tmp = malloc(sizeof(struct MagicalPotatoBall));
    memcpy(tmp, this, sizeof(struct MagicalPotatoBall));
    printf("Magical Potato Ball:\n");
    printf("  weight: %d\n", tmp->base.weight);
    printf("  produced by: %s\n", tmp->base.producer_name);
    printf("  magical effect: %s\n", MagicalEffectToString[tmp->effect]);
    printf("  magic level: %d\n", tmp->effect_level);
    free(tmp);
    return;
};
void potato_ball_dtor(struct PotatoBall **obj) {
    if (*obj == NULL) return;
    free(*obj);
    *obj = NULL;
    return;
}
void magical_potato_ball_dtor(struct PotatoBall **obj) {
    if (*obj == NULL) return;
    free((struct MagicalPotatoBall *)(*obj));
    *obj = NULL;
    return;
}

struct PotatoBall *fry_potato_ball(struct Potato **input) {
    struct PotatoBall *ret = NULL;
    if (*input == NULL) return ret;
    ret = malloc(sizeof(struct PotatoBall));
    ret->weight = (*input)->weight;
    ret->producer_name = (*input)->producer_name;
    ret->dtor = &potato_ball_dtor;
    ret->print = &print_potato_ball;
    potato_dtor(input);
    *input = NULL;
    return ret;
}
struct PotatoBall *fry_magical_potato_ball(struct Potato **input) {
    struct MagicalPotatoBall *ret = NULL;
    if (*input == NULL) return (struct PotatoBall *)ret;
    struct MagicalPotato *tmp;
    tmp = malloc(sizeof(struct MagicalPotato));
    ret = malloc(sizeof(struct MagicalPotatoBall));
    memcpy(tmp, (*input), sizeof(struct MagicalPotato));
    ret->base.producer_name = tmp->base.producer_name;
    ret->base.weight = tmp->base.weight;
    ret->base.dtor = &magical_potato_ball_dtor;
    ret->base.print = &print_magical_potato_ball;
    ret->effect = tmp->effect;
    ret->effect_level = tmp->effect_level;
    magical_potato_dtor(input);
    *input = NULL;
    free(tmp);

    return (struct PotatoBall *)ret;
}

struct Potato *smallten_summon_potato(const struct PotatoProducer *this) {
    struct MagicalPotato *ret = NULL;
    if (this == NULL) return (struct Potato *)ret;
    srand(time(NULL));
    ret = malloc(sizeof(struct MagicalPotato));
    ret->base.print = &print_magical_potato;
    ret->base.fry = &fry_magical_potato_ball;
    ret->base.producer_name = this->name;
    ret->base.weight = (rand() % 7122) + 1;
    ret->effect = rand() % 4;
    ret->effect_level = (rand() % 7122) + 1;
    return (struct Potato *)ret;
}
struct Potato *subarya_produce_potato(const struct PotatoProducer *this) {
    struct Potato *ret = NULL;
    if (this == NULL) return ret;
    srand(time(NULL));
    ret = malloc(sizeof(struct Potato));
    ret->print = &print_potato;
    ret->fry = &fry_potato_ball;
    ret->producer_name = this->name;
    ret->weight = (rand() % 7122) + 1;
    return (struct Potato *)ret;
}

void init_smallten(struct PotatoProducer *obj) {
    obj->produce = &smallten_summon_potato;
    strcpy(obj->name, "smallten");
    return;
}
void init_subarya(struct PotatoProducer *obj) {
    obj->produce = &subarya_produce_potato;
    strcpy(obj->name, "subarya");
    return;
}
