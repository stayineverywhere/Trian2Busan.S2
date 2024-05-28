#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <Windows.h>
#include <stdbool.h>

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
        printf("%d -> %d", oaggro, aggro);
    printf(")\n");
}

void print_dongseok(int omp, int mp, int oaggro, int aggro, int stamina)
{
    // 동석 상태 출력
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
        printf("%d -> %d, ", oaggro, aggro);
    printf("stamina: %d)\n", stamina);
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


int get_dongseok_move(int mp, int zp)
{
    int move;
    //printf("마동석 이동 (0. 대기, 1. 왼쪽으로 이동) >> ");
    while (1) {
        if (mp != zp + 1)
            printf("madongseok move (0:stay, 1:left) >> ");
        else
            printf("madongseok move (0:stay) >> ");
        scanf_s("%d", &move);
        if (mp == zp + 1 && move == 0) break;
        else if (mp != zp + 1 && (move == 0 || move == 1)) break;
    }
    return move;
}

//좀비 행동 함수
int zombie_action(int cp, int mp, int ct_aggro, int ma_aggro, int zp)
{
    int action = ATK_NONE; // 공격 대상 초기화

    // 인간과 인접한 경우
    if (zp == cp - 1 && zp == mp - 1) {
        // 어그로가 높은 쪽을 공격
        if (ct_aggro > ma_aggro)
            action = ATK_CITIZEN; // 시민 공격
        else
            action = ATK_DONGSEOK; // 마동석 공격
    }
    else if (zp == cp - 1) {
        action = ATK_CITIZEN; // 시민 공격
    }
    else if (zp == mp - 1) {
        action = ATK_DONGSEOK; // 마동석 공격
    }
    return action;
}

int get_dongseok_action(int mp, int zp)
{
    int action;
    while (1) {
        if( mp == zp + 1)
            //printf("\n마동석 행동 선택 (0. 휴식, 1. 도발, 2. 붙들기) >> ");
            printf("madongseok action (0:rest, 1:provoke, 2:pull) >> ");
        else
            //printf("\n마동석 행동 선택 (0. 휴식, 1. 도발) >> ");
            printf("madongseok action (0:rest, 1:provoke) >> ");
        scanf_s("%d", &action);
        if (mp == zp+1 && action >= 0 && action <= 2) break;
        else if(mp != zp + 1 && action >= 0 && action <= 1) break;
    }
    return action;
}
// ma_stamina와 ma_aggro를 업데이트하기 위하여, ma_status[2] 배열을 이용하여 업데이트함
// 함수 호출전에 ma_status에 ma_stamina와 ma_aggro를 복사하고, 함수 호출 후에 복원함
int dongseok_action(int prob, int mp, int action, int status[2])
{
    int aggro = status[0];
    int stamina = status[1];
    int zombie_hold = 0;

    if (action == ACTION_REST) {
        //printf("\n마동석이 휴식을 취합니다.\n");
        printf("\nmadongseok rests...\n");
        aggro--;
        if (aggro < AGGRO_MIN)
            aggro = AGGRO_MIN;
        stamina++;
        if (stamina > STM_MAX)
            stamina = STM_MAX;
        printf("madongseok : %d (aggro: %d -> %d, stamina: %d -> %d)\n", mp,
            status[0], aggro, status[1], stamina);
    }
    else if (action == ACTION_PROVOKE) {
        //printf("\n마동석이 좀비를 도발합니다.\n");
        printf("\nmadongseok provoked zombie...\n");
        aggro = AGGRO_MAX;
        printf("madongseok : %d (aggro: %d -> %d, stamina: %d)\n", mp,
            status[0], aggro, stamina);
    }
    else if (action == ACTION_PULL) {
        //printf("\n마동석이 좀비를 붙들기 시도합니다.\n");
        aggro += 2;
        if (aggro > AGGRO_MAX)
            aggro = AGGRO_MAX;
        stamina--;
        if (stamina < STM_MIN)
            stamina = STM_MIN;
        if (rand() % 100 >= prob) {
            //printf("마동석이 좀비를 성공적으로 붙들었습니다.\n");
            printf("madongseok pulled zombie... Next turn, it can't move.\n");
            zombie_hold = 1; // 다음 턴에 좀비 이동 불가능 표시
        }
        else {
            //printf("마동석이 좀비를 붙들지 못했습니다.\n");
            printf("madongseok failed to pull zombie.\n");
        }
        printf("madongseok : %d (aggro: %d -> %d, stamina: %d -> %d)\n", mp,
            status[0], aggro, status[1], stamina);
    }
    status[0] = aggro;
    status[1] = stamina;
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
        //printf("\n<이동 페이즈>\n");
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
        //printf("\n마동석 이동 페이즈\n");
        int move = get_dongseok_move(mp, zp);
        mp = move_dongseok(move, mp);
        ma_aggro = update_aggro(old_ma_aggro=ma_aggro, omp, mp);
        print_train(train_length, cp, zp, mp);
        print_dongseok(omp, mp, old_ma_aggro, ma_aggro, ma_stamina);

        // <행동> 페이즈
        //printf("\n<행동 페이즈>\n");
        // zombie 행동 처리 필요
        // 
        // <행동> 페이즈에서 좀비의 행동 처리
        int z_action = zombie_action(cp, mp, ct_aggro, ma_aggro, zp);
        
        // 좀비의 공격 대상 출력
        printf("Citizen does nothing.\n");
        if (z_action == ATK_CITIZEN) {
            //printf("좀비가 시민을 공격합니다.\n");
            printf("Zombie attacked nobody.\n");
            break;
        }
        else if (z_action == ATK_DONGSEOK) {
            //printf("좀비가 마동석을 공격합니다.\n");
            printf("Zombie attacked madongseok (aggro: %d vs. %d, madongseok stamina: %d -> %d)\n", ct_aggro, ma_aggro, ma_stamina, ma_stamina-1);
            ma_stamina--;
        }
        else { // ATK_NONE
            //printf("좀비가 아무도 공격하지 않습니다.\n");
            printf("Zombie attacked nobody.\n");
        }
                      
        // 마동석 행동 입력 받기
        int action = get_dongseok_action(mp, zp);

        // 마동석 행동 수행
        int ma_status[2] = { ma_aggro, ma_stamina };
        zt = dongseok_action(probability, mp, action, ma_status);
        // DEBUG
        //printf("%d %d ==> %d %d\n", ma_stamina, ma_aggro, ma_status[0], ma_status[1]);
        ma_aggro = ma_status[0];
        ma_stamina = ma_status[1];
                
        if (ma_stamina < STM_MIN) {
            //printf("\n마동석의 체력이 0이 되어 게임이 종료됩니다. GAME OVER!\n");
            printf("GAME OVER! madongseok's stamina(%d) is below than minimum stamina (%)\n", ma_stamina, STM_MIN);
            break;
        }

        // 다음 턴 준비
        Sleep(1000);
    }

    return 0;
}