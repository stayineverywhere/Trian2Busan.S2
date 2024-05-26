#include <stdio.h>
#include <stdlib.h>
#include <time.h>

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
    printf("%s ", prompt, min_val, max_val);
    scanf_s("%d", &value);
    while (value < min_val || value > max_val) {
        printf("%s ", prompt, min_val, max_val);
        scanf_s("%d", &value);
    }
    return value;
}

void print_traIn(int train_length, int cp, int zp, int mp)
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


void print_citizen(int ocp, int cp)
{
    // 시민 상태 출력
    printf("citizen : ");
    if (ocp == cp) {
        printf("stay %d\n", cp);
    }
    else {
        printf("%d -> %d\n", ocp, cp);
    }
}

void print_zombie(int ozp, int zp)
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
        printf("\nSUCCESS!\n");
        printf("citizen(s) escaped to the next train\n");
        return TRUE;
    }
    else if (zp == cp + 1) {
        printf("\nGAME OVER!\n");
        printf("Citizen(s) has(have) been attacked by a zombie\n");
        return TRUE;
    }
    return FALSE;
}
int main(void) {

    // 인트로 화면 출력
    intro();

    // 시민, 좀비, 열차의 길이 및 확률 변수 선언
    int train_length;
    int probability;
    int stamina_ma = STM_MAX; // 마동석 체력
    int aggro_ma = AGGRO_MAX; // 마동석 어그로

    // 시민, 좀비, 열차의 위치 변수 선언
    int mp, zp, cp, omp, ozp, ocp;
    int zt = 0; // 좀비 이동 플래그

    srand(time(NULL));  // 난수 시드 설정

    // 열차의 길이 입력 받기
    train_length = get_value("train length(%d ~ %d)>> ", LEN_MIN, LEN_MAX);
    // 확률 입력 받기
    probability = get_value("percentile probability 'p'(%d ~ %d)>> ", PROB_MIN, PROB_MAX);

    // 마동석, 좀비, 시민의 위치 초기화
    mp = train_length - 2;
    zp = train_length - 3;
    cp = train_length - 6;

    // 열차 상태 출력
    print_traIn(train_length, cp, zp, mp);

    while (1) {
        // <이동> 페이즈
        printf("\n<이동 페이즈>\n");
        // 시민 이동
        printf("\n시민 이동 전\n");
        if (mp != 1) {
            if (rand() % 100 >= probability) {
                cp--;
                if (cp < 1)
                    cp = 1;
                aggro_ma++;
            }
            else {
                aggro_ma--;
                if (aggro_ma < AGGRO_MIN)
                    aggro_ma = AGGRO_MIN;
            }
        }

        // 좀비 이동
        printf("\n좀비 이동 전\n");
        if (zt == 0) {
            if (zp != cp + 1) {
                if ((rand() % 2) == 0 && mp != zp + 1 && mp != zp - 1) {
                    zp--;
                }
                else {
                    if (mp > zp)
                        zp++;
                }
            }
            zt = 1;
        }
        else {
            zt = 0;
        }

        // 열차 상태 출력
        print_train(train_length, cp, zp, mp);
        // 시민 상태 출력
        print_citizen(ocp, cp);
        // 좈비 상태 출력
        print_zombie(ozp, zp);
        // 게임 종료 검사
        if (check_gameover(cp, zp))
            break;


        // 마동석 이동 입력 받기
        printf("\n마동석 이동 페이즈\n");
        int dongseok_move;
        printf("마동석 이동 (0. 대기, 1. 왼쪽으로 이동) >> ");
        scanf_s("%d", &dongseok_move);
        if (dongseok_move == MOVE_LEFT) {
            mp--;
            if (mp < 1)
                mp = 1;
            aggro_ma++;
            if (aggro_ma > AGGRO_MAX)
                aggro_ma = AGGRO_MAX;
        }
        else {
            aggro_ma--;
            if (aggro_ma < AGGRO_MIN)
                aggro_ma = AGGRO_MIN;
        }

        // <행동> 페이즈
        printf("\n<행동 페이즈>\n");

        // 시민 행동
        if (cp == 1) {
            printf("\n탈출구에 도착했습니다. YOU WIN!\n");
            break;
        }

        // 좀비 행동
        if (zp == cp + 1) {
            printf("\n시민이 좀비에게 공격당했습니다. GAME OVER!\n");
            break;
        }

        // 마동석 행동 입력 받기
        int dongseok_action;
        printf("\n마동석 행동 선택 (0. 휴식, 1. 도발, 2. 붙들기) >> ");
        scanf_s("%d", &dongseok_action);

        // 마동석 행동 수행
        if (dongseok_action == ACTION_REST) {
            printf("\n마동석이 휴식을 취합니다.\n");
            aggro_ma--;
            if (aggro_ma < AGGRO_MIN)
                aggro_ma = AGGRO_MIN;
            stamina_ma++;
            if (stamina_ma > STM_MAX)
                stamina_ma = STM_MAX;
        }
        else if (dongseok_action == ACTION_PROVOKE) {
            printf("\n마동석이 좀비를 도발합니다.\n");
            aggro_ma = AGGRO_MAX;
        }
        else if (dongseok_action == ACTION_PULL) {
            printf("\n마동석이 좀비를 붙들기 시도합니다.\n");
            if (rand() % 100 >= probability) {
                printf("마동석이 좀비를 성공적으로 붙들었습니다.\n");
                zt = 1; // 다음 턴에 좀비 이동 불가능 표시
                aggro_ma += 2;
                if (aggro_ma > AGGRO_MAX)
                    aggro_ma = AGGRO_MAX;
                stamina_ma--;
                if (stamina_ma < STM_MIN) {
                    printf("\n마동석의 체력이 0이 되어 게임이 종료됩니다. GAME OVER!\n");
                    break;
                }
            }
            else {
                printf("마동석이 좀비를 붙들지 못했습니다.\n");
                stamina_ma--;
                if (stamina_ma < STM_MIN) {
                    printf("\n마동석의 체력이 0이 되어 게임이 종료됩니다. GAME OVER!\n");
                    break;
                }
            }
        }

        // 다음 턴 준비
        Sleep(1000);
    }

    return 0;
}