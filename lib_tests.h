#ifndef LIB_TESTS_H
#define LIB_TESTS_H

void afficher_titre(char *titre);


int run_test(int boolean, char *description, char* fichier, int line);
#define RUN_TEST(BOOL) run_test(BOOL, #BOOL, __FILE__, __LINE__)

void ok_test(int b);


int cacher_tests(void);
void restaurer_tests(int i);
void montrer_tests(void);


int test_tp1(void);
int test_tp1_obligatoire(void);
int test_tp2(void);
int test_tp3(void);

#endif
