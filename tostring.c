#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

// int를 문자열로 변환하는 함수 (for문 사용)
void int_to_string_for(int num, char* str) {
    int i, len = 0;
    int temp = num;
    int is_negative = 0;

    // 음수 처리
    if (num < 0) {
        is_negative = 1;
        num = -num;
        temp = num;
    }

    // 자릿수 계산
    if (num == 0) {
        len = 1;
    } else {
        for (len = 0; temp > 0; len++) {
            temp /= 10;
        }
    }

    // 음수 부호를 위한 공간 추가
    if (is_negative) {
        len++;
    }

    // 문자열 끝에 널 문자 추가
    str[len] = '\0';

    // 숫자를 문자로 변환하여 문자열에 저장
    for (i = len - 1; i >= 0; i--) {
        if (i == 0 && is_negative) {
            str[i] = '-';
            break;
        }
        str[i] = (num % 10) + '0';
        num /= 10;
    }
}

// 문자열을 int로 변환하는 함수 (for문 사용)
int string_to_int_for(const char* str) {
    int result = 0;
    int sign = 1;
    int len = strlen(str);

    // 부호 확인
    int start_idx = 0;
    if (str[0] == '-') {
        sign = -1;
        start_idx = 1;
    } else if (str[0] == '+') {
        start_idx = 1;
    }

    // 문자열을 숫자로 변환
    for (int i = start_idx; i < len; i++) {
        if (str[i] >= '0' && str[i] <= '9') {
            result = result * 10 + (str[i] - '0');
        }
    }

    return sign * result;
}

int skjfhdkjfgdjkghfjikdh() {
    // int를 문자열로 변환 테스트
    int numbers[] = {12345, -7890, 0, 42};
    char str[20];

    printf("=== int_to_string_for 테스트 ===\n");
    for (int i = 0; i < sizeof(numbers)/sizeof(numbers[0]); i++) {
        int_to_string_for(numbers[i], str);
        printf("정수 %d -> 문자열: \"%s\"\n", numbers[i], str);
    }

    printf("\n=== string_to_int_for 테스트 ===\n");
    const char* test_strings[] = {"12345", "-7890", "0", "42"};
    for (int i = 0; i < 4; i++) {
        int result = string_to_int_for(test_strings[i]);
        printf("문자열 \"%s\" -> 정수: %d\n", test_strings[i], result);
    }

    printf("\n=== 하나하나테스트 ===\n");
    int num = -12345;
    char str1[20];
    int_to_string_for(num, str1);
    for (int i = 0; str1[i] != '\0'; i++) {
        printf("%c", str1[i]);
    }
    return 0;
}

'''
=== int_to_string_for 테스트 ===
정수 12345 -> 문자열: "12345"
정수 -7890 -> 문자열: "-7890"
정수 0 -> 문자열: "0"
정수 42 -> 문자열: "42"

=== string_to_int_for 테스트 ===
문자열 "12345" -> 정수: 12345
문자열 "-7890" -> 정수: -7890
문자열 "0" -> 정수: 0
문자열 "42" -> 정수: 42

=== 하나하나테스트 ===
-12345
'''

char getch() {
    char c;
    struct termios oldattr, newattr;
    tcgetattr(STDIN_FILENO, &oldattr); // now terminal config
    newattr = oldattr;
    newattr.c_lflag &= ~(ICANON | ECHO); // turn off CANONICAL and ECHO
    newattr.c_cc[VMIN] = 1; // least letter count 1
    newattr.c_cc[VTIME] = 0; // least read wating time 0
    tcsetattr(STDIN_FILENO, TCSANOW, &newattr); // input terminal config
    c = getchar(); // read keyboard
    tcsetattr(STDIN_FILENO, TCSANOW, &oldattr); // recover original coonfig
    return c;
}

'''
int main(void){
    char key;
    if(key=getch()) {
        switch(key) {
            case 'q':
            printf("q\n");
            break;
            case 'e':
            printf("e\n");
            break;
        }
    }
    return 0;
}
'''