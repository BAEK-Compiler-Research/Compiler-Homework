#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#define NO_KEYWORDS 31
#define ID_LENGTH 30 // identifier ����  30�� ������ �ȵȴ�~
#define STRING_MAX 200
#define STATEMENT_LENGTH 512 // ���� ������ ������ �ִ뷮. 

enum tsymbol { tnull=-1,
tnot, // 0  <!>
tnotequ, // 1 <!=>
tmod,// 2 <%>
tmodAssign,// 3 <%=>
tident,// 4  identifier(��Ī) ex: ch,cnt ������ ������.
tnumber,// 5  �����̴�.
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
/////////////////////// �ؿ������ʹ� �����~
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

char *keyword[NO_KEYWORDS] = { // �� �������� ��Ʈ������ ���� �迭.
	"const","else","if","int","return","void","while","float","auto","double","short","continue","long",

	"unsigned","union","switch","volatile","struct","typedef","do","for","break","goto","static","default",

	"extern","register","case","signed","char","sizeof"}; // asm�� ������ 31���� ����� �ν�.

	enum tsymbol tnum[NO_KEYWORDS] = { // �� ����� �ش��ϴ� ��ū��ȣ�� ���� �迭.
		tconst, telse, tif, tint, treturn ,tvoid,  twhile, tfloat, tauto,  tdouble,  tshort  ,tcontinue,  tlong,
		//   0  1   2        3         4         5          6       7     8             9              10          11            12  
		tunsigned, tunion, tswitch, tvolatile, tstruct, ttypedef, tdo, tfor, tbreak, tgoto,  tstatic, tdefault,
		//          13            14     15         16           17          18      19       20      21     22            23          24  
		textern, tregister,  tcase,  tsigned,  tchar,  tsizeof};
		//         25          26            27          28          29            30  

		char *Symbol_Table[STRING_MAX]; // �ɺ����̺��� ������ �ɺ����̺� ����.
		static int symbolTableCount=0; // �ɺ����̺� ����Դ��� ī��Ʈ�ϴ� ����ƽ ����
		static int fordebug=0;

		void lexicalError(int n) // ���� ����.
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

		int superLetter(char ch) // ù���ڰ� �����̰ų� ������̳�?
		{
			if(isalpha(ch)||ch=='_') return 1; // ������ 1 isalpha�� �������̸� 1�� ��ȯ.
			else return 0; // Ʋ���� 0
		}
		int superLetterOrDigit(char ch) // ���ʹ��� ���� ���� ���ڴ� ���ڳ� �������̳�?
		{
			if(isalnum(ch) || ch=='_') return 1; // isalnum�� �����̰ų� ���ڹ��� (0~9)�� 1 ���� �ƴϸ� 0
			else return 0;
		}

		//������������� ������� �ν��� ���� 8���� 16���� �Է� ��ȯ �Լ� �������������//
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
		int getIntNum(char *firstCharacter) // �����Է¿� ���� ù ����.
		{
			int num = 0,cnt=0; // num�̶�� ������ ������ �ʱⰪ 0
			int value; // value��� ������ ���� ����.
			char ch; // ĳ������ ����.

			if (firstCharacter[cnt] != '0'){ // �Է¹��� ���ڰ� ����'0'�� �ƴϸ� (������ nonzero digit) 
				ch = firstCharacter[cnt]; // ch�� ���� ���ڸ� �Ѱ��ش�. 
				do { // ��� �ض�
					num= 10*num+(int)(ch-'0'); // num�̶� ������ 
					ch=firstCharacter[++cnt]; // ch�� getchar �ؼ� ���ڸ� �Է�. 
				}while(isdigit(ch)); // �����ε���.  // ��������� �Է¹��� ������ �ܼ� 10���� ��ȯ �����̴�.
			} 

			else { //   ó�� �Է¹��� ���ڰ� 0�� ��� 
				ch = firstCharacter[++cnt]; // ���� �޾Ƽ�

				if((ch>='0')&&(ch<='7')) // �� ���� ���ڰ� 0���� 7������ ���ڹ������ // 8������ �Ҹ���.
				{ do
				{
					num = 8*num + (int)(ch-'0'); // 8���� ��ȯ�۾�.
					ch=firstCharacter[++cnt];
				}while((ch>='0')&&(ch<='7')); 
				}
				else if((ch=='X')||(ch=='x'))
				{ // �Է¹������ڰ� x�̸� // 16������ �Ҹ���.
					ch=firstCharacter[++cnt];
					value = hexValue(ch);
					do{
						num = 16 * num + value; // 8���� ��ȯ�۾�.
						ch=firstCharacter[++cnt];
						value = hexValue(ch);
					}while(value != -1); 
				}
				else if((ch=='B')||(ch=='b'))
				{ //�Է¹��� ���ڰ� B�̸� // 2������ ���̴�~
					ch=firstCharacter[++cnt];
					do{ num=2*num + (int)(ch-'0'); // 2������ȯ�۾�.
					ch=firstCharacter[++cnt];
					}while((ch>='0')&&(ch<='1'));
				}
				else num = 0;
			}
			cnt=cnt-1;

			// ungetc(int,FILE *) �ѱ��ڸ� �о����� �ٽ� ���ư�.
			return num;
		}
		//������������� ������� �ν��� ���� 8���� 16���� �Է� ��ȯ �Լ� �������������//

		//������������� �Ǽ���� �ν��� ���� �Լ� �������������//
		float recur_float(int n) // �Ҽ��κ��� �����ϱ����� �Լ�.
		{
			if(n==1) return 0.1f;
			else return recur_float(n-1)*0.1f;
		}
		float getFloatNum(){ // �Ǽ��� �ν��ϱ� ���� �Լ�.

			int num;
			int num2=1;
			float num3;
			char ch;
			ch=getchar();
			fflush(stdin);
			if(ch>='0'&&ch<='9') // ó���� zero�� ������ ����� ��������.
			{
				if(ch=='0') // �ٵ� �� ���� 0�� ��쿡.
					num=0; // num�� 0���� ����.

				num=(int)(ch-'0'); // ���� ���� �ְ�~
				ch=getchar();// �ٽ� ���ڸ� �Է¹ް�
				fflush(stdin); // ���ۺ���~
				if(ch>='0'&&ch<='9') // digit�̸�.

					do{  
						num = num*10 + ((int)ch-'0'); // 10������ ����(?) �κ� ����.
						ch=getchar();// �ٽ� ���ڸ� �Է¹ް�
						fflush(stdin);
					}while(ch!='.'); // ���� .�� �ԷµǱ� ������.  
				fflush(stdin);
				ch=getchar();// �ٽ� ���ڸ� �Է¹ް�
				fflush(stdin); // ���ۺ���~

				if(ch>='0'&&ch<='9') // digit�̸�.
				{num3=((float)(ch-'0')*0.1f); // ����ڿ� f �Ⱥٿ��ָ� double�� �ν��ϳ���??
				num2++;
				ch=getchar();
				fflush(stdin);
				if(ch>='0'&&ch<='9')
					do{  
						num3 = num3 + ((int)ch-'0')*recur_float(num2); // 10������ ����(?) �κ� ����.
						num2++;
						ch=getchar();// �ٽ� ���ڸ� �Է¹ް�
						fflush(stdin);
					}while(ch>='0'&&ch<='9'); // ���� .�� �ԷµǱ� ������.  
				}
				num3=(float)num+num3;
				return num3;
			}
			return -1;
		}
		//������������� �Ǽ���� �ν��� ���� �Լ� �������������//
		//������������� ���� �ν��� ���� �Լ� �������������//
		char *getString()
		{
			char ch;
			char str[STRING_MAX];
			char *tmp;
			int cnt=0;
			ch=getchar();
			fflush(stdin);
			if(ch=='"') // "�� ������
			{
				ch=getchar();
				fflush(stdin); // �ٽ� �Է��� �޾ƺ��� 

				for(;ch!='"';) // ch�� "�� �ƴѵ��� ��~��
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
		//������������� ���� �ν��� ���� �Լ� �������������//
		struct tokenType
		{
			int number; // ��ū�ѹ�.
			union{
				char id[ID_LENGTH]; // identifier �� ������ ����ü. �Ŀ� symbol table�� ����~
				int num; // int value ���� ����ü.
				float fnum; // float value ���� ����ü.
				char *is_String; // string value ���� ����ü.
			}value;
		}; // tokenType �̶�� ����ü ����. 
		//������������� ��ĳ�� ��ü�κ� ����. �������������//  

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
				do { // �ٹٲ��� ���ö����� ���.
					flag=0; // flag init
					while(isspace(ch=temp[cnt2++])); // ���鹫��
					if(superLetter(ch)) // ù���ڰ� ����� Ȥ�� �������� ���.
					{
						i=0; // i=0���� �ʱ�ȭ.
						do // �����϶�
						{ if(i<ID_LENGTH)id[i++]=ch; // ���� i�� ID�� �ν��Ҽ� �ִ� ���̺��� ������ ID�迭�� �ѱ��ڸ� �־��.
						ch=temp[cnt2++]; // �������ڸ� ch�� �Է��϶�.
						}while(superLetterOrDigit(ch)); // ������ Ȥ�� ���� Ȥ�� ������ε���..

						if(i>=ID_LENGTH)lexicalError(1); // ���̸� �Ѿ�� ������ ������ش�.
						id[i] = '\0'; // ���Ṯ�ڸ� �߰����ش�.
						cnt2=cnt2-1;
						for(index=0;index<NO_KEYWORDS;index++) // Ű���带 �����Ѵ�.
							if(!strcmp(id,keyword[index])) break;   // ��Ī�Ǵ� ���� �߰��� ã���� ���� ��Ż.

						if(index<NO_KEYWORDS){  // index ī���Ͱ� ����� ���� ������ 
							token.number=tnum[index]; // ��ū�ѹ��� �ش��ϴ� ��ū�ѹ��� ������.
							token.value.num=0; // ����� 0����.
						}
						else // ����� ���� ������ 
						{ 
							for(cnt=0;cnt<symbolTableCount;cnt++) // �ɺ����̺��� �˻��Ѵ�
								if(!strcmp(id,Symbol_Table[cnt]))
								{ 
									flag=1; break; // �ɺ����̺� ���� ������ flag=1.
								} 
								switch(flag){
			case 1: token.number=tident; // ��ū�ѹ��� �ش��ϴ� ��ū�ѹ��� ������.
				token.value.num=cnt;
				break;
			case 0:  token.number=tident; // ���̵�Ƽ���̾�� �ν��ϰ�
				Symbol_Table[symbolTableCount] = (char*)malloc(sizeof(char)*ID_LENGTH); //  �ɺ����̺� ����� ���̵�Ƽ���̾� �޸� �Ҵ�.
				strcpy(Symbol_Table[symbolTableCount],id); // id�� �Էµ� identifier �ɺ����̺�� ����.
				symbolTableCount++;
				token.value.num = cnt; // ���̵�Ƽ���̾��� ��ū����� �ɺ����̺��� �ε�����.
				break;
			default : printf("kekkekekekekekekek\n");exit(0);
								}
						}
					}    
					else if (isdigit(ch)){ // ���ڰ� integer�� ���.
						i=0;
						do // �����϶�
						{ imsi[i++]=ch; //
						ch=temp[cnt2++]; // �������ڸ� ch�� �Է��϶�.
						}while(isdigit(ch)||ch=='x'||ch=='X'||ch=='b'||ch=='B'); // �����ε���.
						imsi[i]='\0';
						token.number = tnumber;
						token.value.num = getIntNum(imsi);
						cnt2=cnt2-1;
					}
					else if(temp[cnt2-2] == '\n') break; // ������ ������ ���ͼ� �������ϴ� ī���Ͱ� �޸𸮸� ����Ű�� ��� ó��.
					else switch(ch){

		case '/': ch=temp[cnt2++]; // state 10 �ּ��� �ν�.
			if(ch=='*')
				do{
					while(ch!='*')ch=temp[cnt2++];
					ch=temp[cnt2++]; // �ּ��� ������ �����Ϸ��� ���⼭ �� �� �ִ�.
				}while(ch!='/'); // ��ü�ּ�
			else if(ch=='/'){ // �����ּ�.
				while(ch=temp[cnt2++] !='\n');
				cnt2=cnt2-1;
			} // Ȥ�� �̺κп��� ����������� ����.

			else if(ch =='='){ token.number = tdivAssign;token.value.num = 0;}
			else { token.number=tdiv;token.value.num = 0;
			cnt2=cnt2-1;
			}
			break;
		case '!': ch=temp[cnt2++];
			if(ch=='='){token.number=tnotequ; // ������.
			token.value.num = 0;}
			else { token.number = tnot;token.value.num = 0; // ��.
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
		//������������� ��ĳ�� ��ü �κ� �������������//
		void main()
		{
			int i;
			char ch;
			FILE *dest,*src; // �ҽ����� ��������.
			char buf[STATEMENT_LENGTH]; // �ӽ÷� ���� ����.
			char srcFileName[20], dataFileName[20];
			printf("Source File Name Input : ");
			scanf("%s",srcFileName);
			/*
			// argument�� 3�� �Է����� �ʾ��� ��� ������ �����Ų��.
			if(argc != 3) {
				printf("wrong input! input : [program_name] <source file> <destination file>\n");
				exit(1);
			}
			*/
			// �ι�° argument�� �̿��Ͽ� source-file�� �б� ���� ����. file�� ������ ���� ��� ������ �����Ų��.
			if((src = fopen(srcFileName, "r")) == NULL) {
				printf(" %s file cannot found!!\n " , srcFileName);
				exit(1);
			}
			strcpy(dataFileName,"lex data.txt");
			// ����° argument�� �̿��Ͽ� destination-file�� �����Ѵ�. file�� �������� ���� ��� ������ �����Ų��.
			if((dest = fopen(dataFileName, "w+")) == NULL) {
				printf(" %s file can't create " , dataFileName);
				exit(1);
			}
			// fgets function�� �̿��Ͽ� source-file���� �� �پ� �о�´�. 
			while(!feof(src)) {
				fgets(buf, STATEMENT_LENGTH, src);
				fputs(buf, dest);      // destination-file�� ���
				scanner(buf,dest);         // scanner-function ȣ��
			}// file ���� �����ϸ� while loop�� �������´�.
			//symbol table�� ����� ������ destination-file�� ����Ѵ�.
			fprintf(dest, "\n\n----------------------- [ Symbol Table ] -----------------------\n");

			for( i =0 ; i < symbolTableCount ; i++)
			{
				//identifier�� ��Ī�� �߰ߵ� line�� ����Ѵ�.
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

