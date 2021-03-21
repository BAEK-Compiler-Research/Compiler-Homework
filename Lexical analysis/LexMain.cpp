#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#define NO_KEYWORDS 31
#define ID_LENGTH 30 // identifier 길이  30을 넘으면 안된다~
#define STRING_MAX 200
#define STATEMENT_LENGTH 512 // 한줄 읽을때 적재할 최대량. 

enum tsymbol { tnull=-1,
tnot, // 0  <!>
tnotequ, // 1 <!=>
tmod,// 2 <%>
tmodAssign,// 3 <%=>
tident,// 4  identifier(명칭) ex: ch,cnt 따위의 변수명.
tnumber,// 5  숫자이다.
tand,// 6  <&&>
tlparen,// 7 (
trparen,// 8 )
tmul,// 9 <*>
tmulAssign,// 10 <*=>
tplus,// 11 <+>
tinc,// 12 <++>
taddAssign,// 13 <+=>
tcomma,// 14 <,>
tminus,// 15 <->
tdec,// 16 <-->
tsubAssign,// 17 <
tdiv,// 18 </>
tdivAssign,// 19 </=>
tsemicolon,// 20 <;>
tless,// 21  <
tlesse,// 22 <=
tassign,// 23 =
tequal,// 24 ==
tgreat,// 25 >
tgreate,// 26 >=
tlbracket,// 27 [
trbracket,// 28 ]
teof,// 29 EOF
tlbrace,// 30 {
tor,// 31 ||
trbrace,// 32 }
/////////////////////// 밑에서부터는 예약어~
tconst,// 33
telse,// 34
tif,// 35
tint,// 36
treturn,// 37
tvoid,// 38
twhile,// 39
tfloat,// 40 
tauto,// 41
tdouble,// 42
tshort, // 43
tcontinue, //44
tlong, // 45
tunsigned, //46
tunion, // 47
tswitch, // 48 
tvolatile, // 49 
tstruct, // 50 
ttypedef, //51 
tdo, //52 
tfor, //53
tbreak,  //54
tgoto, //55
tstatic, //56 
tdefault, //57
textern, //58 
tregister, //59 
tcase, //60 
tsigned, //61  
tchar, // 62 
tsizeof}; //63

char *keyword[NO_KEYWORDS] = { // 각 지정어의 스트링값을 갖는 배열.
	"const","else","if","int","return","void","while","float","auto","double","short","continue","long",

	"unsigned","union","switch","volatile","struct","typedef","do","for","break","goto","static","default",

	"extern","register","case","signed","char","sizeof"}; // asm을 제외한 31개의 예약어 인식.

	enum tsymbol tnum[NO_KEYWORDS] = { // 각 지정어에 해당하는 토큰번호를 갖는 배열.
		tconst, telse, tif, tint, treturn ,tvoid,  twhile, tfloat, tauto,  tdouble,  tshort  ,tcontinue,  tlong,
		//   0  1   2        3         4         5          6       7     8             9              10          11            12  
		tunsigned, tunion, tswitch, tvolatile, tstruct, ttypedef, tdo, tfor, tbreak, tgoto,  tstatic, tdefault,
		//          13            14     15         16           17          18      19       20      21     22            23          24  
		textern, tregister,  tcase,  tsigned,  tchar,  tsizeof};
		//         25          26            27          28          29            30  

		char *Symbol_Table[STRING_MAX]; // 심볼테이블을 저장할 심볼테이블 변수.
		static int symbolTableCount=0; // 심볼테이블 몇개나왔는지 카운트하는 스태틱 변수
		static int fordebug=0;

		void lexicalError(int n) // 에러 검출.
		{
			printf(" *** lexical Error : ");
			switch(n)
			{
			case 1: printf("an idetifier length must be less than 12.\n");
				break;
			case 2: printf("next character must be &.\n");
				break;
			case 3: printf("next character must be |.\n");
				break;
			case 4: printf("invalid character!\n");
				break;
			}
		}

		int superLetter(char ch) // 첫문자가 문자이거나 언더바이냐?
		{
			if(isalpha(ch)||ch=='_') return 1; // 맞으면 1 isalpha는 영문자이면 1을 반환.
			else return 0; // 틀리면 0
		}
		int superLetterOrDigit(char ch) // 최초문자 이후 들어온 문자는 숫자나 영문자이냐?
		{
			if(isalnum(ch) || ch=='_') return 1; // isalnum은 문자이거나 숫자범위 (0~9)면 1 리턴 아니면 0
			else return 0;
		}

		//↓↓↓↓↓↓↓↓↓↓↓↓ 정수상수 인식을 위한 8진수 16진수 입력 변환 함수 ↓↓↓↓↓↓↓↓↓↓↓↓//
		int hexValue(char ch)
		{
			switch(ch){
   case '0':case '1':case '2':case '3':case '4':case '5':case '6':case '7':case '8':case '9':
	   return(ch-'0'); break;
   case 'A':case 'B':case 'C':case 'D':case 'E':case 'F':
	   return(ch-'A'+10); break;
   case 'a':case 'b':case 'c':case 'd':case 'e':case 'f':
	   return (ch-'a'+10); break;
   default:return -1;break;
			}
		}
		int getIntNum(char *firstCharacter) // 숫자입력에 들어온 첫 문자.
		{
			int num = 0,cnt=0; // num이라는 정수형 변수에 초기값 0
			int value; // value라는 정수형 변수 선언.
			char ch; // 캐릭터형 변수.

			if (firstCharacter[cnt] != '0'){ // 입력받은 인자가 문자'0'이 아니면 (시작이 nonzero digit) 
				ch = firstCharacter[cnt]; // ch에 받은 인자를 넘겨준다. 
				do { // 계속 해라
					num= 10*num+(int)(ch-'0'); // num이란 변수에 
					ch=firstCharacter[++cnt]; // ch에 getchar 해서 숫자를 입력. 
				}while(isdigit(ch)); // 숫자인동안.  // 여기까지가 입력받은 숫자의 단순 10진수 변환 연산이다.
			} 

			else { //   처음 입력받은 문자가 0인 경우 
				ch = firstCharacter[++cnt]; // 문자 받아서

				if((ch>='0')&&(ch<='7')) // 그 받은 문자가 0에서 7사이의 문자범위라면 // 8진수란 소리다.
				{ do
				{
					num = 8*num + (int)(ch-'0'); // 8진수 변환작업.
					ch=firstCharacter[++cnt];
				}while((ch>='0')&&(ch<='7')); 
				}
				else if((ch=='X')||(ch=='x'))
				{ // 입력받은문자가 x이면 // 16진수란 소리다.
					ch=firstCharacter[++cnt];
					value = hexValue(ch);
					do{
						num = 16 * num + value; // 8진수 변환작업.
						ch=firstCharacter[++cnt];
						value = hexValue(ch);
					}while(value != -1); 
				}
				else if((ch=='B')||(ch=='b'))
				{ //입력받은 문자가 B이면 // 2진수란 말이다~
					ch=firstCharacter[++cnt];
					do{ num=2*num + (int)(ch-'0'); // 2진수변환작업.
					ch=firstCharacter[++cnt];
					}while((ch>='0')&&(ch<='1'));
				}
				else num = 0;
			}
			cnt=cnt-1;

			// ungetc(int,FILE *) 한글자를 읽었던걸 다시 돌아감.
			return num;
		}
		//↑↑↑↑↑↑↑↑↑↑↑↑ 정수상수 인식을 위한 8진수 16진수 입력 변환 함수 ↑↑↑↑↑↑↑↑↑↑↑↑//

		//↓↓↓↓↓↓↓↓↓↓↓↓ 실수상수 인식을 위한 함수 ↓↓↓↓↓↓↓↓↓↓↓↓//
		float recur_float(int n) // 소수부분을 구현하기위한 함수.
		{
			if(n==1) return 0.1f;
			else return recur_float(n-1)*0.1f;
		}
		float getFloatNum(){ // 실수를 인식하기 위한 함수.

			int num;
			int num2=1;
			float num3;
			char ch;
			ch=getchar();
			fflush(stdin);
			if(ch>='0'&&ch<='9') // 처음에 zero를 포함한 어떤수가 들어왔을때.
			{
				if(ch=='0') // 근데 그 수가 0일 경우에.
					num=0; // num은 0으로 셋팅.

				num=(int)(ch-'0'); // 들어온 숫자 넣고~
				ch=getchar();// 다시 문자를 입력받고
				fflush(stdin); // 버퍼비우고~
				if(ch>='0'&&ch<='9') // digit이면.

					do{  
						num = num*10 + ((int)ch-'0'); // 10진수로 지수(?) 부분 생성.
						ch=getchar();// 다시 문자를 입력받고
						fflush(stdin);
					}while(ch!='.'); // 문자 .이 입력되기 전까지.  
				fflush(stdin);
				ch=getchar();// 다시 문자를 입력받고
				fflush(stdin); // 버퍼비우고~

				if(ch>='0'&&ch<='9') // digit이면.
				{num3=((float)(ch-'0')*0.1f); // 상수뒤에 f 안붙여주면 double로 인식하나봐??
				num2++;
				ch=getchar();
				fflush(stdin);
				if(ch>='0'&&ch<='9')
					do{  
						num3 = num3 + ((int)ch-'0')*recur_float(num2); // 10진수로 지수(?) 부분 생성.
						num2++;
						ch=getchar();// 다시 문자를 입력받고
						fflush(stdin);
					}while(ch>='0'&&ch<='9'); // 문자 .이 입력되기 전까지.  
				}
				num3=(float)num+num3;
				return num3;
			}
			return -1;
		}
		//↑↑↑↑↑↑↑↑↑↑↑↑ 실수상수 인식을 위한 함수 ↑↑↑↑↑↑↑↑↑↑↑↑//
		//↓↓↓↓↓↓↓↓↓↓↓↓ 정수 인식을 위한 함수 ↓↓↓↓↓↓↓↓↓↓↓↓//
		char *getString()
		{
			char ch;
			char str[STRING_MAX];
			char *tmp;
			int cnt=0;
			ch=getchar();
			fflush(stdin);
			if(ch=='"') // "가 들어오면
			{
				ch=getchar();
				fflush(stdin); // 다시 입력을 받아보고 

				for(;ch!='"';) // ch가 "가 아닌동안 계~속
				{
					if(ch=='\0') ch = '"';
					str[cnt] = ch;
					cnt++;
					ch=getchar();
					fflush(stdin);
					if(ch=='"')
					{
						// printf("current state : %c\n",str[cnt-1]);
						if(str[cnt-1] == '\\')
						{
							str[cnt]=ch;
							ch='\0';
							continue;
						}
						else break;
					}
				}
				str[cnt] = '\0';
			}
			tmp = (char *)malloc(sizeof(char)*(cnt+1));
			strcpy(tmp,str);

			return tmp;
		}
		//↑↑↑↑↑↑↑↑↑↑↑↑ 정수 인식을 위한 함수 ↑↑↑↑↑↑↑↑↑↑↑↑//
		struct tokenType
		{
			int number; // 토큰넘버.
			union{
				char id[ID_LENGTH]; // identifier 를 저장할 공용체. 후에 symbol table로 고고씽~
				int num; // int value 저장 공용체.
				float fnum; // float value 저장 공용체.
				char *is_String; // string value 저장 공용체.
			}value;
		}; // tokenType 이라는 구조체 선언. 
		//↓↓↓↓↓↓↓↓↓↓↓↓ 스캐너 본체부분 시작. ↓↓↓↓↓↓↓↓↓↓↓↓//  

		struct tokenType scanner(char *buf,FILE *dest)
		{
			struct tokenType token;
			int i,index,flag=0;
			int cnt2=0,cnt=0;
			char ch,id[ID_LENGTH],temp[STRING_MAX],imsi[STRING_MAX];
			strcpy(temp,buf);
			// token.number = tnull
			fprintf(dest,": ");
			if(temp[cnt2]!='\n')
				do { // 줄바꿈이 나올때까지 계속.
					flag=0; // flag init
					while(isspace(ch=temp[cnt2++])); // 공백무시
					if(superLetter(ch)) // 첫문자가 언더바 혹은 영문자의 경우.
					{
						i=0; // i=0으로 초기화.
						do // 실행하라
						{ if(i<ID_LENGTH)id[i++]=ch; // 만약 i가 ID를 인식할수 있는 길이보다 작으면 ID배열에 한글자를 넣어라.
						ch=temp[cnt2++]; // 다음글자를 ch에 입력하라.
						}while(superLetterOrDigit(ch)); // 영문자 혹은 숫자 혹은 언더바인동안..

						if(i>=ID_LENGTH)lexicalError(1); // 길이를 넘어가면 에러를 출력해준다.
						id[i] = '\0'; // 종료문자를 추가해준다.
						cnt2=cnt2-1;
						for(index=0;index<NO_KEYWORDS;index++) // 키워드를 조사한다.
							if(!strcmp(id,keyword[index])) break;   // 매칭되는 예약어를 중간에 찾으면 포문 이탈.

						if(index<NO_KEYWORDS){  // index 카운터가 예약어 내에 있으면 
							token.number=tnum[index]; // 토큰넘버는 해당하는 토큰넘버를 가진다.
							token.value.num=0; // 밸류는 0이지.
						}
						else // 예약어 내에 없으면 
						{ 
							for(cnt=0;cnt<symbolTableCount;cnt++) // 심볼테이블을 검색한다
								if(!strcmp(id,Symbol_Table[cnt]))
								{ 
									flag=1; break; // 심볼테이블 내에 있으면 flag=1.
								} 
								switch(flag){
			case 1: token.number=tident; // 토큰넘버는 해당하는 토큰넘버를 가진다.
				token.value.num=cnt;
				break;
			case 0:  token.number=tident; // 아이덴티파이어로 인식하고
				Symbol_Table[symbolTableCount] = (char*)malloc(sizeof(char)*ID_LENGTH); //  심볼테이블에 저장될 아이덴티파이어 메모리 할당.
				strcpy(Symbol_Table[symbolTableCount],id); // id에 입력된 identifier 심볼테이블로 붙임.
				symbolTableCount++;
				token.value.num = cnt; // 아이덴티파이어의 토큰밸류는 심볼테이블의 인덱스값.
				break;
			default : printf("kekkekekekekekekek\n");exit(0);
								}
						}
					}    
					else if (isdigit(ch)){ // 문자가 integer의 경우.
						i=0;
						do // 실행하라
						{ imsi[i++]=ch; //
						ch=temp[cnt2++]; // 다음글자를 ch에 입력하라.
						}while(isdigit(ch)||ch=='x'||ch=='X'||ch=='b'||ch=='B'); // 숫자인동안.
						imsi[i]='\0';
						token.number = tnumber;
						token.value.num = getIntNum(imsi);
						cnt2=cnt2-1;
					}
					else if(temp[cnt2-2] == '\n') break; // 공백이 여러번 나와서 포인팅하는 카운터가 메모리를 가리키는 경우 처리.
					else switch(ch){

		case '/': ch=temp[cnt2++]; // state 10 주석의 인식.
			if(ch=='*')
				do{
					while(ch!='*')ch=temp[cnt2++];
					ch=temp[cnt2++]; // 주석의 내용을 저장하려면 여기서 할 수 있다.
				}while(ch!='/'); // 전체주석
			else if(ch=='/'){ // 한줄주석.
				while(ch=temp[cnt2++] !='\n');
				cnt2=cnt2-1;
			} // 혹시 이부분에서 에러생길수도 있음.

			else if(ch =='='){ token.number = tdivAssign;token.value.num = 0;}
			else { token.number=tdiv;token.value.num = 0;
			cnt2=cnt2-1;
			}
			break;
		case '!': ch=temp[cnt2++];
			if(ch=='='){token.number=tnotequ; // 낫이퀄.
			token.value.num = 0;}
			else { token.number = tnot;token.value.num = 0; // 낫.
			cnt2=cnt2-1;
			}
			break;

		case '%': ch=temp[cnt2++]; 
			if(ch=='='){
				token.number = tmodAssign;token.value.num = 0;}
			else{
				token.number = tmod;token.value.num = 0;
				cnt2=cnt2-1;
			}
			break;

		case '&': ch=temp[cnt2++];
			if(ch== '&'){token.number=tand;token.value.num = 0;}
			else {lexicalError(2);
			cnt2=cnt2-1;
			}
			break;

		case '*': ch=temp[cnt2++];
			if(ch=='='){token.number = tmulAssign;token.value.num = 0;}
			else{token.number=tmul;token.value.num = 0;
			cnt2=cnt2-1;
			}
			break;

		case '+': ch=temp[cnt2++];
			if(ch=='+') {token.number=tinc;token.value.num = 0; }
			else if(ch=='='){token.number=taddAssign;token.value.num = 0; }
			else {token.number = tplus;token.value.num = 0; 
			cnt2=cnt2-1;
			}
			break;

		case '-': ch=temp[cnt2++];
			if(ch== '-') {token.number=tdec;token.value.num = 0; }
			else if(ch=='='){ token.number=tsubAssign;token.value.num = 0; }
			else{token.number=tminus;token.value.num = 0; 
			cnt2=cnt2-1;
			}
			break;

		case '<': ch=temp[cnt2++];
			if(ch=='=') {token.number=tlesse;token.value.num = 0; }
			else { token.number=tless;token.value.num = 0; 
			cnt2=cnt2-1;
			}
			break;

		case '=': ch=temp[cnt2++];
			if(ch=='=') { token.number=tequal;token.value.num = 0; }
			else{token.number=tassign;token.value.num = 0; 
			cnt2=cnt2-1;
			}
			break;

		case '>': ch=temp[cnt2++];
			if(ch=='=') { token.number=tgreate;token.value.num = 0; }
			else { token.number=tgreat;token.value.num = 0; 
			cnt2=cnt2-1;
			}
			break;
		case '|': ch=temp[cnt2++];
			if(ch=='|'){ token.number = tor; token.value.num = 0; }
			else {lexicalError (3);
			cnt2=cnt2-1;
			}
			break;
		case '(':token.number = tlparen;    token.value.num = 0;  break;
		case ')':token.number = trparen;    token.value.num = 0;  break;
		case ',':token.number = tcomma;  token.value.num = 0;  break;
		case ';':token.number = tsemicolon; token.value.num = 0;  break;
		case '[':token.number = tlbracket; token.value.num = 0;  break;
		case ']':token.number = trbracket; token.value.num = 0;  break;
		case '{':token.number = tlbrace; token.value.num = 0;  break;
		case '}':token.number = trbrace; token.value.num = 0;  break;
		case EOF:token.number = teof;  token.value.num = 0;  break;
		default : {
			printf("Current character : %c",ch);
			lexicalError(4);
			break;
				  }
					}
					fprintf(dest," (%d,%d) ",token.number,token.value.num);
					printf("(%d,%d)\n",token.number,token.value.num);
				}while(temp[cnt2]!='\n');
			fprintf(dest,"\n");
			printf("################LINE %d\n",fordebug++);
			return token;
		}
		//↑↑↑↑↑↑↑↑↑↑↑↑ 스캐너 본체 부분 ↑↑↑↑↑↑↑↑↑↑↑↑//
		void main()
		{
			int i;
			char ch;
			FILE *dest,*src; // 소스파일 최종파일.
			char buf[STATEMENT_LENGTH]; // 임시로 받을 변수.
			char srcFileName[20], dataFileName[20];
			printf("Source File Name Input : ");
			scanf("%s",srcFileName);
			/*
			// argument를 3개 입력하지 않았을 경우 비정상 종료시킨다.
			if(argc != 3) {
				printf("wrong input! input : [program_name] <source file> <destination file>\n");
				exit(1);
			}
			*/
			// 두번째 argument를 이용하여 source-file을 읽기 모드로 연다. file이 열리지 않을 경우 비정상 종료시킨다.
			if((src = fopen(srcFileName, "r")) == NULL) {
				printf(" %s file cannot found!!\n " , srcFileName);
				exit(1);
			}
			strcpy(dataFileName,"lex data.txt");
			// 세번째 argument를 이용하여 destination-file을 생성한다. file이 생성되지 않을 경우 비정상 종료시킨다.
			if((dest = fopen(dataFileName, "w+")) == NULL) {
				printf(" %s file can't create " , dataFileName);
				exit(1);
			}
			// fgets function을 이용하여 source-file에서 한 줄씩 읽어온다. 
			while(!feof(src)) {
				fgets(buf, STATEMENT_LENGTH, src);
				fputs(buf, dest);      // destination-file에 기록
				scanner(buf,dest);         // scanner-function 호출
			}// file 끝에 도달하면 while loop를 빠져나온다.
			//symbol table에 저장된 내용을 destination-file에 기록한다.
			fprintf(dest, "\n\n----------------------- [ Symbol Table ] -----------------------\n");

			for( i =0 ; i < symbolTableCount ; i++)
			{
				//identifier의 명칭과 발견된 line을 출력한다.
				fprintf( dest, "(%d) %5s\n", i, Symbol_Table[i]);
			}

			fclose(dest);
			fclose(src);

			if((dest = fopen(dataFileName, "r")) == NULL) {
				printf(" %s file can't find " , dataFileName);
				exit(1);
			}
			while((ch=fgetc(dest)) !=EOF)
			{
				printf("%c", ch);
			}

			fclose(dest);
		}

