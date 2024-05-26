#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <Windows.h>


//파라미터
#define LEN_MIN 15 // 기차 길이
#define LEN_MAX 50  
#define STM_MIN 0  // 마동석 체력
#define STM_MAX 5
#define PROB_MIN 10 // 확률
#define PROB_MAX 90 
#define AGGRO_MIN 0 //어그로 범위
#define AGGRO_MAX 5

//마동적 이동 방향
#define MOVE_LEFT 1
#define MOVE_STAY 0

//좀비의 공격 대상
#define ATK_NONE 0
#define ATK_CITIZEN 1
#define ATK_DONGSEOK 2

//마동석 행동
#define ACTION_REST 0
#define ACTION_PROVOKE 1
#define ACTION_PULL 2

#define TRUE 1
#define FALSE 0

int update_agro(int current, int move)
{
    if (move > 0) // 왼쪽으로 이동
        return current + 1;
    else // if (move == 0) // 제자리 대기
        return current;
}

void intro()
{
    //인트로
    printf("  *     *          **       *                **      *    * \n");
    printf("  *******        **  **     *****         ********   *    * \n");
    printf("  *     *      **     ***   *               *****    ****** \n");
    printf("  *******                                 **     **  *    * \n");
    printf("                                          **     **  *    * \n");
    printf("**********       **                         ****     *    * \n");
    printf("     *           **                                  *    * \n");
    printf("     *           ************                     **        \n");
    printf("     *                                          **  **      \n");
    printf("     *                                          **  **      \n");
    printf("     *                                            **        \n");

}


int get_value(char prompt[], int min_val, int max_val)
{
    int value;
    printf("%s(%d ~ %d)>> ", prompt, min_val, max_val);
    scanf_s("%d", &value);
    while (value < min_val || value > max_val) {
        printf("%s(%d ~ %d)>> ", prompt, min_val, max_val);
        scanf_s("%d", &value);
    }
    return value;
}

void print_train(int train_length, int cp, int zp, int mp)
{
    // 열차의 초기 상태 출력
    printf("\n");
    for (int i = 0; i < train_length; i++) {
        printf("#");
    }
    printf("\n#");
    for (int i = 1; i < train_length - 1; i++) {
        if (i == cp)
            printf("C");
        else if (i == zp)
            printf("Z");
        else if (i == mp)
            printf("M");
        else
            printf(" ");
    }
    printf("#\n");
    for (int i = 0; i < train_length; i++) {
        printf("#");
    }
    printf("\n");
}


void print_citizen(int ocp, int cp, int oaggro, int aggro)
{
    // 시민 상태 출력
    printf("citizen : ");
    if (ocp == cp) {
        printf("stay %d ", cp);
    }
    else {
        printf("%d -> %d ", ocp, cp);
    }

    printf("(aggro: ");
    if (oaggro == aggro)
        printf("%d", aggro);
    else
        printf("%d->%d", oaggro, aggro);
    printf(")\n");
}

void print_dongseok(int omp, int mp, int oaggro, int aggro, int ostamina, int stamina)
{
    // 시민 상태 출력
    printf("madongseok : ");
    if (omp == mp) {
        printf("stay %d ", mp);
    }
    else {
        printf("%d -> %d ", omp, mp);
    }

    printf("(aggro: ");
    if (oaggro == aggro)
        printf("%d, ", aggro);
    else
        printf("%d->%d, ", oaggro, aggro);
    printf("stamina: ");
    if (ostamina == stamina)
        printf("%d ", stamina);
    else
        printf("%d->%d ", ostamina, stamina);
    printf(")\n");
}

void print_zombie(int zt, int ozp, int zp)
{
    // 좀비 상태 출력
    printf("zombie  : ");
    if (zt == 1) {
        if (ozp == zp)
            printf("stay %d\n", zp);
        else
            printf("%d -> %d\n", ozp, zp);

    }
    else {
        printf("stay %d (cannot move)\n", zp);
    }

}

int check_gameover(int cp, int zp)
{
    if (cp == 1) {
        printf("\nSUCCESS! YOU WIN\n");
        printf("Citizen(s) escaped to the next train\n");
        return TRUE;
    }
    else if (zp == cp + 1) {
        printf("\nGAME OVER!\n");
        printf("Citizen(s) has(have) been attacked by a zombie\n");
        return TRUE;
    }
    return FALSE;
}

int move_citizen(int prob, int cp)
{
    if (rand() % 100 >= prob) {
        cp--;
    }
    return cp;
}

int update_aggro(int aggro, int old, int current)
{
    if (old != current)
        aggro++;
    else
        aggro--;
    if (aggro > AGGRO_MAX)
        aggro = AGGRO_MAX;
    if (aggro < AGGRO_MIN)
        aggro = AGGRO_MIN;
    return aggro;
}

int move_zombie(int prob, int zp)
{
    if (rand() % 100 < prob)
        zp--;
    return zp;
}

int move_dongseok(int move, int mp)
{
    if (move == MOVE_LEFT)
        mp--;
    return mp;
}


int dongseok_action(int prob, int action, int ma_status[2])
{
    int ma_stamina = ma_status[0];
    int ma_aggro = ma_status[1];
    int zombie_hold = 0;

    if (action == ACTION_REST) {
        printf("\n마동석이 휴식을 취합니다.\n");
        ma_aggro--;
        if (ma_aggro < AGGRO_MIN)
            ma_aggro = AGGRO_MIN;
        ma_stamina++;
        if (ma_stamina > STM_MAX)
            ma_stamina = STM_MAX;
    }
    else if (action == ACTION_PROVOKE) {
        printf("\n마동석이 좀비를 도발합니다.\n");
        ma_aggro = AGGRO_MAX;
    }
    else if (action == ACTION_PULL) {
        printf("\n마동석이 좀비를 붙들기 시도합니다.\n");
        ma_aggro += 2;
        if (ma_aggro > AGGRO_MAX)
            ma_aggro = AGGRO_MAX;
        ma_stamina--;
        if (rand() % 100 >= prob) {
            printf("마동석이 좀비를 성공적으로 붙들었습니다.\n");
            zombie_hold = 1; // 다음 턴에 좀비 이동 불가능 표시
        }
        else {
            printf("마동석이 좀비를 붙들지 못했습니다.\n");
        }
    }
    ma_status[0] = ma_stamina;
    ma_status[1] = ma_aggro;
    return zombie_hold;

}

int main(void) {

    // 인트로 화면 출력
    intro();

    // 시민, 좀비, 열차의 길이 및 확률 변수 선언
    int train_length;
    int probability;
    int ma_stamina; // 마동석 체력
    int ma_aggro = 1; // 마동석 어그로
    int ct_aggro = 1; // 시만 어그로
    int old_ma_aggro, old_ct_aggro;

    // 시민, 좀비, 열차의 위치 변수 선언
    int mp, zp, cp, omp, ozp, ocp;
    int zt = 0; // 좀비 이동 플래그

    srand(time(NULL));  // 난수 시드 설정

    // 열차의 길이 입력 받기
    train_length = get_value("train length ", LEN_MIN, LEN_MAX);
    // 마동석 스태미나
    ma_stamina = get_value("madongseok stamina ", STM_MIN, STM_MAX);
    // 확률 입력 받기
    probability = get_value("percentile probability 'p' ", PROB_MIN, PROB_MAX);

    // 마동석, 좀비, 시민의 위치 초기화
    mp = train_length - 2;
    zp = train_length - 3;
    cp = train_length - 6;

    // 열차 상태 출력
    print_train(train_length, cp, zp, mp);

    while (1) {
        // 현재 위치 보관
        ocp = cp;
        ozp = zp;
        omp = mp;

        // <이동> 페이즈
        printf("\n<이동 페이즈>\n");
        // 시민 이동
        cp = move_citizen(100-probability, cp);
        ct_aggro = update_aggro(old_ct_aggro=ct_aggro, ocp, cp);

        // 좀비 이동
        if (zt == 0) {
            zp = move_zombie(probability, zp);
            zt = 1;
        }
        else {
            zt = 0;
        }

        // 열차 상태 출력
        print_train(train_length, cp, zp, mp);
        // 시민 상태 출력
        print_citizen(ocp, cp, old_ct_aggro, ct_aggro);
        // 좀비 상태 출력
        print_zombie(zt, ozp, zp);
        // 게임 종료 검사
        if (check_gameover(cp, zp))
            break;


        // 마동석 이동 입력 받기
        printf("\n마동석 이동 페이즈\n");
        int move;
        printf("마동석 이동 (0. 대기, 1. 왼쪽으로 이동) >> ");
        scanf_s("%d", &move);
        mp = move_dongseok(move, mp);
        ma_aggro = update_aggro(old_ma_aggro=ma_aggro, omp, mp);

        // <행동> 페이즈
        printf("\n<행동 페이즈>\n");
        // zombie 행동 처리 필요
        // 
        // **********************
        // 
        // 마동석 행동 입력 받기
        int action;
        printf("\n마동석 행동 선택 (0. 휴식, 1. 도발, 2. 붙들기) >> ");
        scanf_s("%d", &action);

        // 마동석 행동 수행
        int ma_status[2] = { ma_stamina, ma_aggro };
        int old_ma_stamina = ma_stamina;
        zt = dongseok_action(probability, action, ma_status);
        printf("%d %d ==> %d %d\n", ma_stamina, ma_aggro, ma_status[0], ma_status[1]);
        ma_stamina = ma_status[0];
        ma_aggro = ma_status[1];

        print_dongseok(omp, mp, old_ma_aggro, ma_aggro, old_ma_stamina, ma_stamina);
        if (ma_stamina < STM_MIN) {
            printf("\n마동석의 체력이 0이 되어 게임이 종료됩니다. GAME OVER!\n");
            break;
        }

        // 다음 턴 준비
        Sleep(1000);
    }

    return 0;
}