/***********************interpret.c******************************/
/* Soubor: interpret.c - interpretacia jazyka IFJ14  			*/
/* Předmět: Formalní jazyky a překladače (IFJ) 					*/
/* Projekt: Implementace interpretu imperativního jazyka IFJ14  */
/* Varianta zadání: b/1/I 										*/
/* Datum: prosinec 2014											*/
/* Kódování: UTF-8												*/
/* Autoři:			Matúš Bútora (xbutor01)						*/
/*					Roman Jaška  (xjaska00)						*/
/*					Filip Kalous (xkalou03)						*/
/****************************************************************/

int kanter=0;
bool lastbool;
bool vypocet = false;
int readyInt = 0;
double readyDou = 0;
int resArrIntIndex = -1;
int resArrDouIndex = -1;
double lastdouble = 0;
char *laststring = NULL;
char *tmpstring = NULL;
void * lastAdr = NULL;
void * lastAdr1 = NULL;
bool recycleAdr = false;
int * currentInstType;
bool realOnly = false;



char *concate(char *s1, char *s2)
{
	int len1 = (int)strlen(s1);
	int len2 = (int)strlen(s2);

	char * result = malloc(sizeof(char)*(len1+len2));
	if(result == NULL)	errorHandler(errInt);
	
	InsertLastMarius(&Smetisko, result);
	memset (result, 0, len1+len2);
	strcpy (result, s1);
	strcat (result, s2);
	return result;
}

void * preklopenie (tInsList *currIL)
{
	
	struct ListItem *origActiv = currIL -> active;
	First(currIL);
	while (currIL -> active != NULL)
			{
				currentInstType = & (currIL -> active -> instruction . instype);

				if ( * currentInstType >= I_ADDI   &&
					 * currentInstType <= I_NEQUAL &&
					 * currentInstType != I_ASGNI  &&
					 * currentInstType != I_ASGNS  &&
					 * currentInstType != I_ASGNR  &&
					 * currentInstType != I_ASGNB   )
				{
					if (currIL -> active -> instruction . adr1 != NULL)
						((tNodePtr)currIL -> active -> instruction . adr1)->data->used = false;
					if (currIL -> active -> instruction . adr2 != NULL)
						((tNodePtr)currIL -> active -> instruction . adr2)->data->used = false;
				
				}

			Succ(currIL);
			}
	currIL->active=origActiv;
}

//Funkcia na prehodenie hodnot z integroveho pola do pola double
void intArr2douArr (int * intArr, double * douArr, int count)
{
	for (int i = 0; i < count; ++i)
		(douArr [i]) = ((double) (intArr [i]));
	resArrDouIndex = resArrIntIndex;
	readyDou = (double) readyInt;

}

int interpret(tNodePtr *TS, tInsList *currIL)	//precitaj si zadanie real %g, atd
{

	tContent conOld[100];
	tContent conVarOld;
	tNodePtr currentTerm; 
	int l;
	int r;
	int cmp;
	
	tInstruction *new;
	tNodePtr temp;
	tNodePtr temp2;

	tNodePtr A1 = NULL;
	tNodePtr A2 = NULL;

	int longestExpressionLength = 0; //dlzka najdlhsieho vyrazu
	int currentExpressionLength = 0; //dlzka aktualneho vyrazu
	First(currIL);


		//Prechadzanie vsetkych instrukcii z listu pre analyza
		while (currIL -> active != NULL)
		{
			currentInstType = & (currIL -> active -> instruction . instype);
			
			// Nájdenie najdlhšieho výrazu (počet operátorov)

			if ( * currentInstType >= I_ADDI   &&
				 * currentInstType <= I_NEQUAL &&
				 * currentInstType != I_ASGNI  &&
				 * currentInstType != I_ASGNS  &&
				 * currentInstType != I_ASGNR  &&
				 * currentInstType != I_ASGNB   )
				
				{
					currentExpressionLength++;
					if (recycleAdr == true)
					{
						if (currIL -> active -> instruction . adr1 != NULL)							
						{
							((tNodePtr)currIL -> active -> instruction . adr1)->data->used = false;
						}
						if (currIL -> active -> instruction . adr2 != NULL)
						{
							((tNodePtr)currIL -> active -> instruction . adr2)->data->used = false;
						}
					}
				}
			else
			{
				
				if ( currentExpressionLength > longestExpressionLength)
					longestExpressionLength = currentExpressionLength;
				currentExpressionLength = 0;
			}

		Succ(currIL);
		}
		if (recycleAdr==true) recycleAdr=false;
	
	new = NULL;



	int resArrInt [longestExpressionLength];
	double resArrDou [longestExpressionLength];

	First(currIL);

	
	do
	{

		new = Copy(currIL);
		
		switch(new->instype)
		{
            case I_NOP: break;
							
							//ARITMETICKE OPERACIE//
			case I_ADDI:
				A1 = (tNodePtr) new -> adr1;
				A2 = (tNodePtr) new -> adr2;

				// Prisla nam len jedna adresa => priradenie

				if (A2 == NULL)
				{

					if (A1 -> data -> content . initialized == false) 
						errorHandler (errRunUnin);

						readyInt = A1 -> data -> content . integer;

					break;
				}

				// Prisli nam dve nove adresy => zaciatok noveho medzivypoctu

				if (A1 -> data -> used == false && A2 -> data -> used == false)
				{

					if (A1 -> data -> content . initialized == false ||
						A2 -> data -> content . initialized == false  ) 
						errorHandler (errRunUnin);
					if (realOnly == false)
					{
						resArrIntIndex++;
						resArrInt [resArrIntIndex] = A1 -> data -> content . integer + A2 -> data -> content . integer;
						A1 -> data -> used = true;
						A2 -> data -> used = true;
						readyInt = resArrInt [resArrIntIndex];
					}
					else
					{
						resArrDouIndex++;
						resArrDou [resArrDouIndex] = (double) (A1 -> data -> content . integer) + (double) (A2 -> data -> content . integer);
						A1 -> data -> used = true;
						A2 -> data -> used = true;
						readyDou = resArrDou [resArrDouIndex];
					}
					
					break;
				}

				// Prva adresa nebola pouzita ale druha ano => pouzijem prvu

				if (A1 -> data -> used == false && A2 -> data -> used == true)
				{	

					if (A1 -> data -> content . initialized == false) 
						errorHandler (errRunUnin);

					if (realOnly == false)
					{ 
						resArrInt [resArrIntIndex] += A1 -> data -> content . integer;
						A1 -> data -> used = true;
						readyInt = resArrInt [resArrIntIndex];
					}
					else
					{
						resArrDou [resArrDouIndex] += (double) (A1 -> data -> content . integer);
						A1 -> data -> used = true;
						readyDou = resArrDou [resArrDouIndex];
					}

					break;
				}

				// Prva adresa uz bola druha nie  => pouzijem druhu

				if (A1 -> data -> used == true && A2 -> data -> used == false)
				{

					if (A2 -> data -> content . initialized == false) 
						errorHandler (errRunUnin);
					if (realOnly == false)
					{
						resArrInt [resArrIntIndex] += A2 -> data -> content . integer;
						A2 -> data -> used = true;
						readyInt = resArrInt [resArrIntIndex];
					}
					else
					{
						resArrDou [resArrDouIndex] += (double) (A2 -> data -> content . integer);
						A2 -> data -> used = true;
						readyDou = resArrDou [resArrDouIndex];
					}
					
					break;
				}

				// Obe adresy boli pouzite => zratam medzivysledky

				if (A1 -> data -> used == true && A2 -> data -> used == true)
				{

					if (realOnly == false)
					{
						if (resArrIntIndex-1 != -1)
						{
							resArrInt [resArrIntIndex-1] += resArrInt [resArrIntIndex];
							resArrIntIndex--;
							readyInt = resArrInt [resArrIntIndex];
						}
						else
							readyInt = resArrInt [resArrIntIndex];
					}
					else
					{
						if (resArrDouIndex-1 != -1)
						{
							resArrDou [resArrDouIndex-1] += resArrDou [resArrDouIndex];
							resArrDouIndex--;
							readyDou = resArrDou [resArrDouIndex];
						}
						else
							readyDou = resArrDou [resArrDouIndex];
					}

					break;
				}

				errorHandler (errRunRest);

				break;
			
			case I_ADDR:
			if (realOnly==false)
			{
				realOnly = true;
				intArr2douArr(&(resArrInt[0]),&(resArrDou[0]),longestExpressionLength);
			}
				
				A1 = (tNodePtr) new -> adr1;
				A2 = (tNodePtr) new -> adr2;

				if (A1 -> data -> type == t_expr_int || A1 -> data -> type == sym_var_int )
					A1 -> data -> content . real = (double) A1 -> data -> content . integer;

				// Prisla nam len jedna adresa => priradenie

				if (A2 == NULL)
				{

					if (A1 -> data -> content . initialized == false) 
						errorHandler (errRunUnin);

					readyDou = A1 -> data -> content . real;

					break;
				}

				if (A2 -> data -> type == t_expr_int || A2 -> data -> type == sym_var_int )
					A2 -> data -> content . real = (double) A2 -> data -> content . integer;

				// Prisli nam dve nove adresy => zaciatok noveho medzivypoctu

				if (A1 -> data -> used == false && A2 -> data -> used == false)
				{

					if (A1 -> data -> content . initialized == false ||
						A2 -> data -> content . initialized == false  ) 
						errorHandler (errRunUnin);

					resArrDouIndex++;
					resArrDou [resArrDouIndex] = A1 -> data -> content . real + A2 -> data -> content . real;
					A1 -> data -> used = true;
					A2 -> data -> used = true;
					readyDou = resArrDou [resArrDouIndex];

					break;
				}

				// Prva adresa nebola pouzita ale druha ano => pouzijem prvu

				if (A1 -> data -> used == false && A2 -> data -> used == true)
				{	

					if (A1 -> data -> content . initialized == false) 
						errorHandler (errRunUnin);

					resArrDou [resArrDouIndex] += A1 -> data -> content . real;
					A1 -> data -> used = true;
					readyDou = resArrDou [resArrDouIndex];

					break;
				}

				// Prva adresa uz bola druha nie  => pouzijem druhu

				if (A1 -> data -> used == true && A2 -> data -> used == false)
				{

					if (A2 -> data -> content . initialized == false) 
						errorHandler (errRunUnin);

					resArrDou [resArrDouIndex] += A2 -> data -> content . real;
					A2 -> data -> used = true;
					readyDou = resArrDou [resArrDouIndex];

					break;
				}

				// Obe adresy boli pouzite => zratam medzivysledky

				if (A1 -> data -> used == true && A2 -> data -> used == true)
				{

					if (resArrDouIndex-1 != -1)
					{
						resArrDou [resArrDouIndex-1] += resArrDou [resArrDouIndex];
						resArrDouIndex--;
						readyDou = resArrDou [resArrDouIndex];
					}
					else
						readyDou = resArrDou [resArrDouIndex];

					break;
				}

				errorHandler (errRunRest);

				break;
			
			case I_CONCATE:		
				if(((tNodePtr) new->adr2) == NULL)
				{
					temp = ((tNodePtr) new->adr1);

					laststring = malloc(sizeof (char)* (int)strlen(temp->data->content.string));
					if(laststring == NULL)	errorHandler(errInt);
					InsertLastMarius(&Smetisko, laststring);

					memset(laststring, 0, strlen(laststring));
					strcat(laststring, temp->data->content.string);
				}
				else
				{
					if(vypocet == false)
					{
						temp = ((tNodePtr) new->adr1);
						temp2 = ((tNodePtr) new->adr2);
			
						laststring =  concate(temp->data->content.string, temp2->data->content.string);			
						vypocet = true;
					} else
					{
						temp2 = ((tNodePtr) new->adr2);
						laststring = concate(laststring, temp2->data->content.string);
					}
				}
				
				break;

			case I_SUBI:
				A1 = (tNodePtr) new -> adr1;
				A2 = (tNodePtr) new -> adr2;

				// Prisli nam dve nove adresy => zaciatok noveho medzivypoctu

				if (A1 -> data -> used == false && A2 -> data -> used == false)
				{
					if (A1 -> data -> content . initialized == false ||
						A2 -> data -> content . initialized == false  )
					{
						fprintf(stderr, "Variable '%s' uninitialized.\n", A1 -> data -> name);
						errorHandler (errRunUnin);
					}

					if (realOnly == false)
					{
						resArrIntIndex++;
						resArrInt [resArrIntIndex] = A1 -> data -> content . integer - A2 -> data -> content . integer;
						A1 -> data -> used = true;
						A2 -> data -> used = true;
						readyInt = resArrInt [resArrIntIndex];
					}
					else
					{
						resArrDouIndex++;
						resArrDou [resArrDouIndex] = (double) (A1 -> data -> content . integer) - (double) (A2 -> data -> content . integer);
						A1 -> data -> used = true;
						A2 -> data -> used = true;
						readyDou = resArrDou [resArrDouIndex];
					}
					break;
				}

				// Prva adresa nebola pouzita ale druha ano => pouzijem prvu

				if (A1 -> data -> used == false && A2 -> data -> used == true)
				{	
					if (A1 -> data -> content . initialized == false) 
					{
						fprintf(stderr, "Variable '%s' uninitialized.\n", A1 -> data -> name);
						errorHandler (errRunUnin);
					}

					if (realOnly == false)
					{
						resArrInt [resArrIntIndex] -= A1 -> data -> content . integer;
						A1 -> data -> used = true;
						readyInt = resArrInt [resArrIntIndex];
					}
					else
					{
						resArrDou [resArrDouIndex] -= (double) (A1 -> data -> content . integer);
						A1 -> data -> used = true;
						readyDou = resArrDou [resArrDouIndex];
					}

					break;
				}

				// Prva adresa uz bola druha nie  => pouzijem druhu

				if (A1 -> data -> used == true && A2 -> data -> used == false)
				{
					if (A2 -> data -> content . initialized == false) 
					{
						fprintf(stderr, "Variable '%s' uninitialized.\n", A1 -> data -> name);
						errorHandler (errRunUnin);
					}

					if (realOnly == false)
					{
						resArrInt [resArrIntIndex] -= A2 -> data -> content . integer;
						A2 -> data -> used = true;
						readyInt = resArrInt [resArrIntIndex];
					}
					else
					{
						resArrDou [resArrDouIndex] -= (double) (A2 -> data -> content . integer);
						A2 -> data -> used = true;
						readyDou = resArrDou [resArrDouIndex];
					}
					break;
				}

				// Obe adresy boli pouzite => zratam medzivysledky

				if (A1 -> data -> used == true && A2 -> data -> used == true)
				{

					if (realOnly == false)
					{
						if (resArrIntIndex-1 != -1)
						{
							resArrInt [resArrIntIndex-1] -= resArrInt [resArrIntIndex];
							resArrIntIndex--;
							readyInt = resArrInt [resArrIntIndex];
						}
						else
							readyInt = resArrInt [resArrIntIndex];
					}
					else
					{
						if (resArrDouIndex-1 != -1)
						{
							resArrDou [resArrDouIndex-1] -= resArrDou [resArrDouIndex];
							resArrDouIndex--;
							readyDou = resArrDou [resArrDouIndex];
						}
						else
							readyDou = resArrDou [resArrDouIndex];
					}

					break;
				}

				errorHandler (errRunRest);

				break;

			case I_SUBR:
				if (realOnly==false)
				{
					realOnly = true;
					intArr2douArr(&(resArrInt[0]),&(resArrDou[0]),longestExpressionLength);
				}

				A1 = (tNodePtr) new -> adr1;
				A2 = (tNodePtr) new -> adr2;

				if (A1 -> data -> type == t_expr_int || A1 -> data -> type == sym_var_int )
					A1 -> data -> content . real = (double) A1 -> data -> content . integer;
				if (A2 -> data -> type == t_expr_int || A2 -> data -> type == sym_var_int )
					A2 -> data -> content . real = (double) A2 -> data -> content . integer;

				// Prisli nam dve nove adresy => zaciatok noveho medzivypoctu

				if (A1 -> data -> used == false && A2 -> data -> used == false)
				{

					if (A1 -> data -> content . initialized == false ||
						A2 -> data -> content . initialized == false  ) 
						errorHandler (errRunUnin);

					resArrDouIndex++;
					resArrDou [resArrDouIndex] = A1 -> data -> content . real - A2 -> data -> content . real;
					A1 -> data -> used = true;
					A2 -> data -> used = true;
					readyDou = resArrDou [resArrDouIndex];

					break;
				}

				// Prva adresa nebola pouzita ale druha ano => pouzijem prvu

				if (A1 -> data -> used == false && A2 -> data -> used == true)
				{	

					if (A1 -> data -> content . initialized == false) 
						errorHandler (errRunUnin);

					resArrDou [resArrDouIndex] -= A1 -> data -> content . real;
					A1 -> data -> used = true;
					readyDou = resArrDou [resArrDouIndex];

					break;
				}

				// Prva adresa uz bola druha nie  => pouzijem druhu

				if (A1 -> data -> used == true && A2 -> data -> used == false)
				{

					if (A2 -> data -> content . initialized == false) 
						errorHandler (errRunUnin);

					resArrDou [resArrDouIndex] -= A2 -> data -> content . real;
					A2 -> data -> used = true;
					readyDou = resArrDou [resArrDouIndex];

					break;
				}

				// Obe adresy boli pouzite => zratam medzivysledky

				if (A1 -> data -> used == true && A2 -> data -> used == true)
				{
					if (resArrDouIndex-1 != -1)
					{
						resArrDou [resArrDouIndex-1] -= resArrDou [resArrDouIndex];
						resArrDouIndex--;
						readyDou = resArrDou [resArrDouIndex];
					}
					else
						readyDou = resArrDou [resArrDouIndex];

					break;
				}

				errorHandler (errRunRest);

				break;

			case I_MULI:
				A1 = (tNodePtr) new -> adr1;
				A2 = (tNodePtr) new -> adr2;

				// Prisli nam dve nove adresy => zaciatok noveho medzivypoctu

				if (A1 -> data -> used == false && A2 -> data -> used == false)
				{

					if (A1 -> data -> content . initialized == false ||
						A2 -> data -> content . initialized == false  ) 
						errorHandler (errRunUnin);
					if (realOnly == false)
					{
						resArrIntIndex++;
						resArrInt [resArrIntIndex] = A1 -> data -> content . integer * A2 -> data -> content . integer;
						A1 -> data -> used = true;
						A2 -> data -> used = true;
						readyInt = resArrInt [resArrIntIndex];
					}
					else
					{
						resArrDouIndex++;
						resArrDou [resArrDouIndex] = (double) (A1 -> data -> content . integer) * (double) (A2 -> data -> content . integer);
						A1 -> data -> used = true;
						A2 -> data -> used = true;
						readyDou = resArrDou [resArrDouIndex];
					}
					break;
				}

				// Prva adresa nebola pouzita ale druha ano => pouzijem prvu

				if (A1 -> data -> used == false && A2 -> data -> used == true)
				{	

					if (A1 -> data -> content . initialized == false) 
						errorHandler (errRunUnin);

					if ( realOnly == false)
					{
						resArrInt [resArrIntIndex] *= A1 -> data -> content . integer;
						A1 -> data -> used = true;
						readyInt = resArrInt [resArrIntIndex];
					}
					else
					{
						resArrDou [resArrDouIndex] *= (double) (A1 -> data -> content . integer);
						A1 -> data -> used = true;
						readyDou = resArrDou [resArrDouIndex];
					}

					break;
				}

				// Prva adresa uz bola druha nie  => pouzijem druhu

				if (A1 -> data -> used == true && A2 -> data -> used == false)
				{

					if (A2 -> data -> content . initialized == false) 
						errorHandler (errRunUnin);
					if (realOnly == false)
					{
						resArrInt [resArrIntIndex] *= A2 -> data -> content . integer;
						A2 -> data -> used = true;
						readyInt = resArrInt [resArrIntIndex];
					}
					else
					{
						resArrDou [resArrDouIndex] *= (double) (A2 -> data -> content . integer);
						A2 -> data -> used = true;
						readyDou = resArrDou [resArrDouIndex];
					}

					break;
				}

				// Obe adresy boli pouzite => zratam medzivysledky

				if (A1 -> data -> used == true && A2 -> data -> used == true)
				{

					if (realOnly == false)
					{
						if (resArrIntIndex-1 != -1)
						{
							resArrInt [resArrIntIndex-1] *= resArrInt [resArrIntIndex];
							resArrIntIndex--;
							readyInt = resArrInt [resArrIntIndex];
						}
						else
							readyInt = resArrInt [resArrIntIndex];
					}
					else
					{
						if (resArrDouIndex-1 != -1)
						{
							resArrDou [resArrDouIndex-1] *= resArrDou [resArrDouIndex];
							resArrDouIndex--;
							readyDou = resArrDou [resArrDouIndex];
						}
						else
							readyDou = resArrDou [resArrDouIndex];
					}

					break;
				}

				errorHandler (errRunRest);

				break;
			
			case I_MULR:
				if (realOnly==false)
				{
					realOnly = true;
					intArr2douArr(&(resArrInt[0]),&(resArrDou[0]),longestExpressionLength);
				}

				A1 = (tNodePtr) new -> adr1;
				A2 = (tNodePtr) new -> adr2;

				if (A1 -> data -> type == t_expr_int || A1 -> data -> type == sym_var_int )
					A1 -> data -> content . real = (double) A1 -> data -> content . integer;
				if (A2 -> data -> type == t_expr_int || A2 -> data -> type == sym_var_int )
					A2 -> data -> content . real = (double) A2 -> data -> content . integer;

				// Prisli nam dve nove adresy => zaciatok noveho medzivypoctu

				if (A1 -> data -> used == false && A2 -> data -> used == false)
				{

					if (A1 -> data -> content . initialized == false ||
						A2 -> data -> content . initialized == false  ) 
						errorHandler (errRunUnin);

					resArrDouIndex++;
					resArrDou [resArrDouIndex] = A1 -> data -> content . real * A2 -> data -> content . real;
					A1 -> data -> used = true;
					A2 -> data -> used = true;
					readyDou = resArrDou [resArrDouIndex];

					break;
				}

				// Prva adresa nebola pouzita ale druha ano => pouzijem prvu

				if (A1 -> data -> used == false && A2 -> data -> used == true)
				{	

					if (A1 -> data -> content . initialized == false) 
						errorHandler (errRunUnin);

					resArrDou [resArrDouIndex] *= A1 -> data -> content . real;
					A1 -> data -> used = true;
					readyDou = resArrDou [resArrDouIndex];

					break;
				}

				// Prva adresa uz bola druha nie  => pouzijem druhu

				if (A1 -> data -> used == true && A2 -> data -> used == false)
				{

					if (A2 -> data -> content . initialized == false) 
						errorHandler (errRunUnin);

					resArrDou [resArrDouIndex] *= A2 -> data -> content . real;
					A2 -> data -> used = true;
					readyDou = resArrDou [resArrDouIndex];

					break;
				}

				// Obe adresy boli pouzite => zratam medzivysledky

				if (A1 -> data -> used == true && A2 -> data -> used == true)
				{

					if (resArrDouIndex-1 != -1)
					{
						resArrDou [resArrDouIndex-1] *= resArrDou [resArrDouIndex];
						resArrDouIndex--;
						readyDou = resArrDou [resArrDouIndex];
					}
					else
						readyDou = resArrDou [resArrDouIndex];

					break;
				}

				errorHandler (errRunRest);

				break;

			case I_DIVR:

				if (realOnly==false)
				{
					realOnly = true;
					intArr2douArr(&(resArrInt[0]),&(resArrDou[0]),longestExpressionLength);
				}

				A1 = (tNodePtr) new -> adr1;
				A2 = (tNodePtr) new -> adr2;

				if (A1 -> data -> type == t_expr_int || A1 -> data -> type == sym_var_int )
					A1 -> data -> content . real = (double) A1 -> data -> content . integer;
				
				if (A2 != NULL)
				{
					if (A2 -> data -> type == t_expr_int || A2 -> data -> type == sym_var_int )
						A2 -> data -> content . real = (double) A2 -> data -> content . integer;
				}
				

				// Prisli nam dve nove adresy => zaciatok noveho medzivypoctu

				if (A1 -> data -> used == false && A2 -> data -> used == false)
				{

					if (A1 -> data -> content . initialized == false ||
						A2 -> data -> content . initialized == false  ) 
						errorHandler (errRunUnin);

					resArrDouIndex++;
					if (A2 -> data -> content . real == 0) errorHandler (errRunZdiv);
					resArrDou [resArrDouIndex] = A1 -> data -> content . real / A2 -> data -> content . real;
					A1 -> data -> used = true;
					A2 -> data -> used = true;
					readyDou = resArrDou [resArrDouIndex];

					break;
				}

				// Prva adresa nebola pouzita ale druha ano => pouzijem prvu

				if (A1 -> data -> used == false && A2 -> data -> used == true)
				{	

					if (A1 -> data -> content . initialized == false) 
						errorHandler (errRunUnin);

					if (A1 -> data -> content . real == 0) errorHandler (errRunZdiv);

					resArrDou [resArrDouIndex] = resArrDou [resArrDouIndex] / A1 -> data -> content . real;
					A1 -> data -> used = true;
					readyDou = resArrDou [resArrDouIndex];

					break;
				}

				// Prva adresa uz bola druha nie  => pouzijem druhu

				if (A1 -> data -> used == true && A2 -> data -> used == false)
				{

					if (A2 -> data -> content . initialized == false) 
						errorHandler (errRunUnin);
					
					if (A2 -> data -> content . real == 0) errorHandler (errRunZdiv);

					resArrDou [resArrDouIndex] = resArrDou [resArrDouIndex] / A2 -> data -> content . real;
					A2 -> data -> used = true;
					readyDou = resArrDou [resArrDouIndex];

					break;
				}

				// Obe adresy boli pouzite => zratam medzivysledky

				if (A1 -> data -> used == true && A2 -> data -> used == true)
				{

					if (resArrDouIndex-1 != -1)
					{
						if (resArrDou [resArrDouIndex] == 0) errorHandler (errRunZdiv);
						resArrDou [resArrDouIndex-1] = resArrDou [resArrDouIndex-1] / resArrDou [resArrDouIndex];
						resArrDouIndex--;
						readyDou = resArrDou [resArrDouIndex];
					}
					else
						readyDou = resArrDou [resArrDouIndex];

					break;
				}

				errorHandler (errRunRest);

				break;
			
			case I_ASGNI:						
				if (readyInt < 0) errorHandler(errRunRest);	

				((tData) new->result)->content.integer = readyInt;
				((tData) new->result)->content.initialized = true;		
				readyInt = 0;
				vypocet = false;
				resArrIntIndex = -1;
				preklopenie(currIL);

				break;		

			case I_ASGNR:
				if(readyDou < 0) errorHandler(errRunRest);	

				((tData) new->result)->content.real = readyDou;
				((tData) new->result)->content.initialized = true;				
				readyDou = 0;
				vypocet = false;
				resArrIntIndex = -1;
				preklopenie(currIL);

				break;

			case I_ASGNS:			
				(((tData) new->result)->content.string) = malloc(sizeof(char) * strlen(laststring));
				if(((tData) new->result)->content.string == NULL)	errorHandler(errInt);
				InsertLastMarius(&Smetisko, (((tData) new->result)->content.string));
				
				strcpy((((tData) new->result)->content.string), laststring);
				
				laststring = NULL;
				vypocet =  false;
					
				break;

			case I_ASGNB:
				(((tData) new->result)->content.boolean) = lastbool;

				break;		
								//LOGICKE OPERACIE//
			case I_MORE:
			
				A1 = (tNodePtr) new -> adr1;
				A2 = (tNodePtr) new -> adr2;

				// Prisli nam dve nove adresy => zaciatok noveho medzivypoctu

				if (A1 -> data -> used == false && A2 -> data -> used == false)
				{

					if (A1 -> data -> content . initialized == false ||
						A2 -> data -> content . initialized == false  ) 
						errorHandler (errRunUnin);

					
					if ((A1 -> data -> type == t_expr_int || A1 -> data -> type == sym_var_int) &&
						(A2 -> data -> type == t_expr_int || A2 -> data -> type == sym_var_int))
					{	
						lastbool = (A1 -> data -> content . integer > A2 -> data -> content . integer ) ? true : false;
					}

					if ((A1 -> data -> type == t_expr_dou || A1 -> data -> type == sym_var_rea) &&
						(A2 -> data -> type == t_expr_dou || A2 -> data -> type == sym_var_rea))
					{	
						lastbool = (A1 -> data -> content . real > A2 -> data -> content . real ) ? true : false;
					}

					if ((A1 -> data -> type == t_expr_str || A1 -> data -> type == sym_var_str) &&
						(A2 -> data -> type == t_expr_str || A2 -> data -> type == sym_var_str))
					{	
						cmp = strcmp (A1 -> data -> content . string, A2 -> data -> content . string);
						lastbool = (cmp > 0) ? true : false;
					}

					if ((A1 -> data -> type == t_expr_boo || A1 -> data -> type == sym_var_boo) &&
						(A2 -> data -> type == t_expr_boo || A2 -> data -> type == sym_var_boo))
					{	
						lastbool = (A1 -> data -> content . boolean > A2 -> data -> content . boolean) ? true : false;
					}
					A1 -> data -> used = true;
					A2 -> data -> used = true;

					break;
				}

				// Prva adresa nebola pouzita ale druha ano => pouzijem prvu

				if (A1 -> data -> used == false && A2 -> data -> used == true)
				{	

					if (A1 -> data -> content . initialized == false) 
						errorHandler (errRunUnin);

					if (A1 -> data -> type == t_expr_int || A1 -> data -> type == sym_var_int)
					{	
						lastbool = (A1 -> data -> content . integer > resArrInt [resArrIntIndex] ) ? true : false;
					}

					if (A1 -> data -> type == t_expr_dou || A1 -> data -> type == sym_var_rea)
					{	
						lastbool = (A1 -> data -> content . real > resArrDou [resArrDouIndex] ) ? true : false;
					}

					A1 -> data -> used = true;

					break;
				}

				// Prva adresa uz bola druha nie  => pouzijem druhu

				
				if (A1 -> data -> used == true && A2 -> data -> used == false)
				{	

					if (A2 -> data -> content . initialized == false) 
						errorHandler (errRunUnin);

					if (A2 -> data -> type == t_expr_int || A2 -> data -> type == sym_var_int)
					{	
						lastbool = ( resArrInt [resArrIntIndex] > A2 -> data -> content . integer ) ? true : false;
					}

					if (A2 -> data -> type == t_expr_dou || A2 -> data -> type == sym_var_rea)
					{	
						lastbool = ( resArrDou [resArrDouIndex] > A2 -> data -> content . real ) ? true : false;
					}

					A2 -> data -> used = true;

					break;
				}

				// Obe adresy boli pouzite => zratam medzivysledky

				if (A1 -> data -> used == true && A2 -> data -> used == true)
				{

					if 	((A1 -> data -> type == t_expr_int || A1 -> data -> type == sym_var_int) &&
						 (A2 -> data -> type == t_expr_int || A2 -> data -> type == sym_var_int)  )
					{
						if (realOnly == true)
							lastbool = ( resArrDou [resArrDouIndex-1] > resArrDou [resArrDouIndex] ) ? true : false;
						else
							lastbool = ( resArrInt [resArrIntIndex-1] > resArrInt [resArrIntIndex] ) ? true : false;
					}

					if 	((A1 -> data -> type == t_expr_dou || A1 -> data -> type == sym_var_rea) &&
						 (A2 -> data -> type == t_expr_dou || A2 -> data -> type == sym_var_rea)  )
					{
						lastbool = ( resArrDou [resArrDouIndex-1] > resArrDou [resArrDouIndex] ) ? true : false;
					}

					if 	((A1 -> data -> type == t_expr_dou || A1 -> data -> type == sym_var_rea) &&
						 (A2 -> data -> type == t_expr_int || A2 -> data -> type == sym_var_int)  )
					{
						if (realOnly == true)
							lastbool = ( resArrDou [resArrDouIndex-1] > resArrDou [resArrDouIndex] ) ? true : false;
						else
							lastbool = ( resArrInt [resArrIntIndex-1] > resArrInt [resArrIntIndex] ) ? true : false;
					}

					if 	((A1 -> data -> type == t_expr_int || A1 -> data -> type == sym_var_int) &&
						 (A2 -> data -> type == t_expr_dou || A2 -> data -> type == sym_var_rea)  )
					{
						if (realOnly == true)
							lastbool = ( resArrDou [resArrDouIndex-1] > resArrDou [resArrDouIndex] ) ? true : false;
						else
							lastbool = ( resArrInt [resArrIntIndex-1] > resArrInt [resArrIntIndex] ) ? true : false;
					}

					break;
				}

				errorHandler (errRunRest);

				break;
			
			case I_LESS:


				A1 = (tNodePtr) new -> adr1;
				A2 = (tNodePtr) new -> adr2;

				// Prisli nam dve nove adresy => zaciatok noveho medzivypoctu

				if (A1 -> data -> used == false && A2 -> data -> used == false)
				{

					if (A1 -> data -> content . initialized == false ||
						A2 -> data -> content . initialized == false  ) 
						errorHandler (errRunUnin);

					
					if ((A1 -> data -> type == t_expr_int || A1 -> data -> type == sym_var_int) &&
						(A2 -> data -> type == t_expr_int || A2 -> data -> type == sym_var_int))
					{	
						lastbool = (A1 -> data -> content . integer < A2 -> data -> content . integer ) ? true : false;
					}

					if ((A1 -> data -> type == t_expr_dou || A1 -> data -> type == sym_var_rea) &&
						(A2 -> data -> type == t_expr_dou || A2 -> data -> type == sym_var_rea))
					{	
						lastbool = (A1 -> data -> content . real < A2 -> data -> content . real ) ? true : false;
					}

					if ((A1 -> data -> type == t_expr_str || A1 -> data -> type == sym_var_str) &&
						(A2 -> data -> type == t_expr_str || A2 -> data -> type == sym_var_str))
					{	
						cmp = strcmp (A1 -> data -> content . string, A2 -> data -> content . string);
						lastbool = (cmp < 0) ? true : false;
					}

					if ((A1 -> data -> type == t_expr_boo || A1 -> data -> type == sym_var_boo) &&
						(A2 -> data -> type == t_expr_boo || A2 -> data -> type == sym_var_boo))
					{	
						lastbool = (A1 -> data -> content . boolean < A2 -> data -> content . boolean) ? true : false;
					}
					A1 -> data -> used = true;
					A2 -> data -> used = true;

					break;
				}

				// Prva adresa nebola pouzita ale druha ano => pouzijem prvu

				if (A1 -> data -> used == false && A2 -> data -> used == true)
				{	

					if (A1 -> data -> content . initialized == false) 
						errorHandler (errRunUnin);

					if (A1 -> data -> type == t_expr_int || A1 -> data -> type == sym_var_int)
					{	
						lastbool = (A1 -> data -> content . integer < resArrInt [resArrIntIndex] ) ? true : false;
					}

					if (A1 -> data -> type == t_expr_dou || A1 -> data -> type == sym_var_rea)
					{	
						lastbool = (A1 -> data -> content . real < resArrDou [resArrDouIndex] ) ? true : false;
					}

					A1 -> data -> used = true;

					break;
				}

				// Prva adresa uz bola druha nie  => pouzijem druhu

				
				if (A1 -> data -> used == true && A2 -> data -> used == false)
				{	

					if (A2 -> data -> content . initialized == false) 
						errorHandler (errRunUnin);

					if (A2 -> data -> type == t_expr_int || A2 -> data -> type == sym_var_int)
					{	
						lastbool = ( resArrInt [resArrIntIndex] < A2 -> data -> content . integer ) ? true : false;
					}

					if (A2 -> data -> type == t_expr_dou || A2 -> data -> type == sym_var_rea)
					{	
						lastbool = ( resArrDou [resArrDouIndex] < A2 -> data -> content . real ) ? true : false;
					}

					A2 -> data -> used = true;

					break;
				}

				// Obe adresy boli pouzite => zratam medzivysledky

				if (A1 -> data -> used == true && A2 -> data -> used == true)
				{

					if 	((A1 -> data -> type == t_expr_int || A1 -> data -> type == sym_var_int) &&
						 (A2 -> data -> type == t_expr_int || A2 -> data -> type == sym_var_int)  )
					{
						if (realOnly == true)
							lastbool = ( resArrDou [resArrDouIndex-1] < resArrDou [resArrDouIndex] ) ? true : false;
						else
							lastbool = ( resArrInt [resArrIntIndex-1] < resArrInt [resArrIntIndex] ) ? true : false;
					}

					if 	((A1 -> data -> type == t_expr_dou || A1 -> data -> type == sym_var_rea) &&
						 (A2 -> data -> type == t_expr_dou || A2 -> data -> type == sym_var_rea)  )
					{
						lastbool = ( resArrDou [resArrDouIndex-1] < resArrDou [resArrDouIndex] ) ? true : false;
					}

					if 	((A1 -> data -> type == t_expr_dou || A1 -> data -> type == sym_var_rea) &&
						 (A2 -> data -> type == t_expr_int || A2 -> data -> type == sym_var_int)  )
					{
						if (realOnly == true)
							lastbool = ( resArrDou [resArrDouIndex-1] < resArrDou [resArrDouIndex] ) ? true : false;
						else
							lastbool = ( resArrInt [resArrIntIndex-1] < resArrInt [resArrIntIndex] ) ? true : false;
					}

					if 	((A1 -> data -> type == t_expr_int || A1 -> data -> type == sym_var_int) &&
						 (A2 -> data -> type == t_expr_dou || A2 -> data -> type == sym_var_rea)  )
					{
						if (realOnly == true)
							lastbool = ( resArrDou [resArrDouIndex-1] < resArrDou [resArrDouIndex] ) ? true : false;
						else
							lastbool = ( resArrInt [resArrIntIndex-1] < resArrInt [resArrIntIndex] ) ? true : false;
					}

					break;
				}

				errorHandler (errRunRest);

				break;

			case I_EMORE:
				
							
				A1 = (tNodePtr) new -> adr1;
				A2 = (tNodePtr) new -> adr2;

				// Prisli nam dve nove adresy => zaciatok noveho medzivypoctu

				if (A1 -> data -> used == false && A2 -> data -> used == false)
				{

					if (A1 -> data -> content . initialized == false ||
						A2 -> data -> content . initialized == false  ) 
						errorHandler (errRunUnin);

					
					if ((A1 -> data -> type == t_expr_int || A1 -> data -> type == sym_var_int) &&
						(A2 -> data -> type == t_expr_int || A2 -> data -> type == sym_var_int))
					{	
						lastbool = (A1 -> data -> content . integer >= A2 -> data -> content . integer ) ? true : false;
					}

					if ((A1 -> data -> type == t_expr_dou || A1 -> data -> type == sym_var_rea) &&
						(A2 -> data -> type == t_expr_dou || A2 -> data -> type == sym_var_rea))
					{	
						lastbool = (A1 -> data -> content . real >= A2 -> data -> content . real ) ? true : false;
					}

					if ((A1 -> data -> type == t_expr_str || A1 -> data -> type == sym_var_str) &&
						(A2 -> data -> type == t_expr_str || A2 -> data -> type == sym_var_str))
					{	
						cmp = strcmp (A1 -> data -> content . string, A2 -> data -> content . string);
						lastbool = (cmp >= 0) ? true : false;
					}

					if ((A1 -> data -> type == t_expr_boo || A1 -> data -> type == sym_var_boo) &&
						(A2 -> data -> type == t_expr_boo || A2 -> data -> type == sym_var_boo))
					{	
						lastbool = (A1 -> data -> content . boolean >= A2 -> data -> content . boolean) ? true : false;
					}
					A1 -> data -> used = true;
					A2 -> data -> used = true;

					break;
				}

				// Prva adresa nebola pouzita ale druha ano => pouzijem prvu

				if (A1 -> data -> used == false && A2 -> data -> used == true)
				{	

					if (A1 -> data -> content . initialized == false) 
						errorHandler (errRunUnin);

					if (A1 -> data -> type == t_expr_int || A1 -> data -> type == sym_var_int)
					{	
						lastbool = (A1 -> data -> content . integer >= resArrInt [resArrIntIndex] ) ? true : false;
					}

					if (A1 -> data -> type == t_expr_dou || A1 -> data -> type == sym_var_rea)
					{	
						lastbool = (A1 -> data -> content . real >= resArrDou [resArrDouIndex] ) ? true : false;
					}

					A1 -> data -> used = true;

					break;
				}

				// Prva adresa uz bola druha nie  => pouzijem druhu

				
				if (A1 -> data -> used == true && A2 -> data -> used == false)
				{	

					if (A2 -> data -> content . initialized == false) 
						errorHandler (errRunUnin);

					if (A2 -> data -> type == t_expr_int || A2 -> data -> type == sym_var_int)
					{	
						lastbool = ( resArrInt [resArrIntIndex] >= A2 -> data -> content . integer ) ? true : false;
					}

					if (A2 -> data -> type == t_expr_dou || A2 -> data -> type == sym_var_rea)
					{	
						lastbool = ( resArrDou [resArrDouIndex] >= A2 -> data -> content . real ) ? true : false;
					}

					A2 -> data -> used = true;

					break;
				}

				// Obe adresy boli pouzite => zratam medzivysledky

				if (A1 -> data -> used == true && A2 -> data -> used == true)
				{

					if 	((A1 -> data -> type == t_expr_int || A1 -> data -> type == sym_var_int) &&
						 (A2 -> data -> type == t_expr_int || A2 -> data -> type == sym_var_int)  )
					{
						if (realOnly == true)
							lastbool = ( resArrDou [resArrDouIndex-1] >= resArrDou [resArrDouIndex] ) ? true : false;
						else
							lastbool = ( resArrInt [resArrIntIndex-1] >= resArrInt [resArrIntIndex] ) ? true : false;
					}

					if 	((A1 -> data -> type == t_expr_dou || A1 -> data -> type == sym_var_rea) &&
						 (A2 -> data -> type == t_expr_dou || A2 -> data -> type == sym_var_rea)  )
					{
						lastbool = ( resArrDou [resArrDouIndex-1] >= resArrDou [resArrDouIndex] ) ? true : false;
					}

					if 	((A1 -> data -> type == t_expr_dou || A1 -> data -> type == sym_var_rea) &&
						 (A2 -> data -> type == t_expr_int || A2 -> data -> type == sym_var_int)  )
					{
						if (realOnly == true)
							lastbool = ( resArrDou [resArrDouIndex-1] >= resArrDou [resArrDouIndex] ) ? true : false;
						else
							lastbool = ( resArrInt [resArrIntIndex-1] >= resArrInt [resArrIntIndex] ) ? true : false;
					}

					if 	((A1 -> data -> type == t_expr_int || A1 -> data -> type == sym_var_int) &&
						 (A2 -> data -> type == t_expr_dou || A2 -> data -> type == sym_var_rea)  )
					{
						if (realOnly == true)
							lastbool = ( resArrDou [resArrDouIndex-1] >= resArrDou [resArrDouIndex] ) ? true : false;
						else
							lastbool = ( resArrInt [resArrIntIndex-1] >= resArrInt [resArrIntIndex] ) ? true : false;
					}

					break;
				}

				errorHandler (errRunRest);

				break;

			case I_ELESS:
				
							
				A1 = (tNodePtr) new -> adr1;
				A2 = (tNodePtr) new -> adr2;

				// Prisli nam dve nove adresy => zaciatok noveho medzivypoctu

				if (A1 -> data -> used == false && A2 -> data -> used == false)
				{

					if (A1 -> data -> content . initialized == false ||
						A2 -> data -> content . initialized == false  ) 
						errorHandler (errRunUnin);

					
					if ((A1 -> data -> type == t_expr_int || A1 -> data -> type == sym_var_int) &&
						(A2 -> data -> type == t_expr_int || A2 -> data -> type == sym_var_int))
					{	
						lastbool = (A1 -> data -> content . integer <= A2 -> data -> content . integer ) ? true : false;
					}

					if ((A1 -> data -> type == t_expr_dou || A1 -> data -> type == sym_var_rea) &&
						(A2 -> data -> type == t_expr_dou || A2 -> data -> type == sym_var_rea))
					{	
						lastbool = (A1 -> data -> content . real <= A2 -> data -> content . real ) ? true : false;
					}

					if ((A1 -> data -> type == t_expr_str || A1 -> data -> type == sym_var_str) &&
						(A2 -> data -> type == t_expr_str || A2 -> data -> type == sym_var_str))
					{	
						cmp = strcmp (A1 -> data -> content . string, A2 -> data -> content . string);
						lastbool = (cmp <= 0) ? true : false;
					}

					if ((A1 -> data -> type == t_expr_boo || A1 -> data -> type == sym_var_boo) &&
						(A2 -> data -> type == t_expr_boo || A2 -> data -> type == sym_var_boo))
					{	
						lastbool = (A1 -> data -> content . boolean <= A2 -> data -> content . boolean) ? true : false;
					}
					A1 -> data -> used = true;
					A2 -> data -> used = true;

					break;
				}

				// Prva adresa nebola pouzita ale druha ano => pouzijem prvu

				if (A1 -> data -> used == false && A2 -> data -> used == true)
				{	

					if (A1 -> data -> content . initialized == false) 
						errorHandler (errRunUnin);

					if (A1 -> data -> type == t_expr_int || A1 -> data -> type == sym_var_int)
					{	
						lastbool = (A1 -> data -> content . integer <= resArrInt [resArrIntIndex] ) ? true : false;
					}

					if (A1 -> data -> type == t_expr_dou || A1 -> data -> type == sym_var_rea)
					{	
						lastbool = (A1 -> data -> content . real <= resArrDou [resArrDouIndex] ) ? true : false;
					}

					A1 -> data -> used = true;
					
					break;
				}

				// Prva adresa uz bola druha nie  => pouzijem druhu

				
				if (A1 -> data -> used == true && A2 -> data -> used == false)
				{	
	
					if (A2 -> data -> content . initialized == false) 
						errorHandler (errRunUnin);

					if (A2 -> data -> type == t_expr_int || A2 -> data -> type == sym_var_int)
					{	
						lastbool = ( resArrInt [resArrIntIndex] <= A2 -> data -> content . integer ) ? true : false;
					}

					if (A2 -> data -> type == t_expr_dou || A2 -> data -> type == sym_var_rea)
					{	
						lastbool = ( resArrDou [resArrDouIndex] <= A2 -> data -> content . real ) ? true : false;
					}

					A2 -> data -> used = true;

					break;
				}

				// Obe adresy boli pouzite => zratam medzivysledky

				if (A1 -> data -> used == true && A2 -> data -> used == true)
				{

					if 	((A1 -> data -> type == t_expr_int || A1 -> data -> type == sym_var_int) &&
						 (A2 -> data -> type == t_expr_int || A2 -> data -> type == sym_var_int)  )
					{
						if (realOnly == true)
							lastbool = ( resArrDou [resArrDouIndex-1] <= resArrDou [resArrDouIndex] ) ? true : false;
						else
							lastbool = ( resArrInt [resArrIntIndex-1] <= resArrInt [resArrIntIndex] ) ? true : false;
					}

					if 	((A1 -> data -> type == t_expr_dou || A1 -> data -> type == sym_var_rea) &&
						 (A2 -> data -> type == t_expr_dou || A2 -> data -> type == sym_var_rea)  )
					{
						lastbool = ( resArrDou [resArrDouIndex-1] <= resArrDou [resArrDouIndex] ) ? true : false;
					}

					if 	((A1 -> data -> type == t_expr_dou || A1 -> data -> type == sym_var_rea) &&
						 (A2 -> data -> type == t_expr_int || A2 -> data -> type == sym_var_int)  )
					{
						if (realOnly == true)
							lastbool = ( resArrDou [resArrDouIndex-1] <= resArrDou [resArrDouIndex] ) ? true : false;
						else
							lastbool = ( resArrInt [resArrIntIndex-1] <= resArrInt [resArrIntIndex] ) ? true : false;
					}

					if 	((A1 -> data -> type == t_expr_int || A1 -> data -> type == sym_var_int) &&
						 (A2 -> data -> type == t_expr_dou || A2 -> data -> type == sym_var_rea)  )
					{
						if (realOnly == true)
							lastbool = ( resArrDou [resArrDouIndex-1] <= resArrDou [resArrDouIndex] ) ? true : false;
						else
							lastbool = ( resArrInt [resArrIntIndex-1] <= resArrInt [resArrIntIndex] ) ? true : false;
					}

					break;
				}

				errorHandler (errRunRest);

				break;

			case I_EQUAL:
				
							
				A1 = (tNodePtr) new -> adr1;
				A2 = (tNodePtr) new -> adr2;

				// Prisli nam dve nove adresy => zaciatok noveho medzivypoctu

				if (A1 -> data -> used == false && A2 -> data -> used == false)
				{

					if (A1 -> data -> content . initialized == false ||
						A2 -> data -> content . initialized == false  ) 
						errorHandler (errRunUnin);

					
					if ((A1 -> data -> type == t_expr_int || A1 -> data -> type == sym_var_int) &&
						(A2 -> data -> type == t_expr_int || A2 -> data -> type == sym_var_int))
					{	
						lastbool = (A1 -> data -> content . integer == A2 -> data -> content . integer ) ? true : false;
					}

					if ((A1 -> data -> type == t_expr_dou || A1 -> data -> type == sym_var_rea) &&
						(A2 -> data -> type == t_expr_dou || A2 -> data -> type == sym_var_rea))
					{	
						lastbool = (A1 -> data -> content . real == A2 -> data -> content . real ) ? true : false;
					}

					if ((A1 -> data -> type == t_expr_str || A1 -> data -> type == sym_var_str) &&
						(A2 -> data -> type == t_expr_str || A2 -> data -> type == sym_var_str))
					{	
						cmp = strcmp (A1 -> data -> content . string, A2 -> data -> content . string);
						lastbool = (cmp == 0) ? true : false;
					}

					if ((A1 -> data -> type == t_expr_boo || A1 -> data -> type == sym_var_boo) &&
						(A2 -> data -> type == t_expr_boo || A2 -> data -> type == sym_var_boo))
					{	
						lastbool = (A1 -> data -> content . boolean == A2 -> data -> content . boolean) ? true : false;
					}
					A1 -> data -> used = true;
					A2 -> data -> used = true;


					break;
				}

				// Prva adresa nebola pouzita ale druha ano => pouzijem prvu

				if (A1 -> data -> used == false && A2 -> data -> used == true)
				{	

					if (A1 -> data -> content . initialized == false) 
						errorHandler (errRunUnin);

					if (A1 -> data -> type == t_expr_int || A1 -> data -> type == sym_var_int)
					{	
						lastbool = (A1 -> data -> content . integer == resArrInt [resArrIntIndex] ) ? true : false;
					}

					if (A1 -> data -> type == t_expr_dou || A1 -> data -> type == sym_var_rea)
					{	
						lastbool = (A1 -> data -> content . real == resArrDou [resArrDouIndex] ) ? true : false;
					}

					A1 -> data -> used = true;

					break;
				}

				// Prva adresa uz bola druha nie  => pouzijem druhu

				
				if (A1 -> data -> used == true && A2 -> data -> used == false)
				{	

					if (A2 -> data -> content . initialized == false) 
						errorHandler (errRunUnin);

					if (A2 -> data -> type == t_expr_int || A2 -> data -> type == sym_var_int)
					{	
						lastbool = ( resArrInt [resArrIntIndex] == A2 -> data -> content . integer ) ? true : false;
					}

					if (A2 -> data -> type == t_expr_dou || A2 -> data -> type == sym_var_rea)
					{	
						lastbool = ( resArrDou [resArrDouIndex] == A2 -> data -> content . real ) ? true : false;
					}

					A2 -> data -> used = true;

					break;
				}

				// Obe adresy boli pouzite => zratam medzivysledky

				if (A1 -> data -> used == true && A2 -> data -> used == true)
				{

					if 	((A1 -> data -> type == t_expr_int || A1 -> data -> type == sym_var_int) &&
						 (A2 -> data -> type == t_expr_int || A2 -> data -> type == sym_var_int)  )
					{
						if (realOnly == true)
							lastbool = ( resArrDou [resArrDouIndex-1] == resArrDou [resArrDouIndex] ) ? true : false;
						else
							lastbool = ( resArrInt [resArrIntIndex-1] == resArrInt [resArrIntIndex] ) ? true : false;
					}

					if 	((A1 -> data -> type == t_expr_dou || A1 -> data -> type == sym_var_rea) &&
						 (A2 -> data -> type == t_expr_dou || A2 -> data -> type == sym_var_rea)  )
					{
						lastbool = ( resArrDou [resArrDouIndex-1] == resArrDou [resArrDouIndex] ) ? true : false;
					}

					if 	((A1 -> data -> type == t_expr_dou || A1 -> data -> type == sym_var_rea) &&
						 (A2 -> data -> type == t_expr_int || A2 -> data -> type == sym_var_int)  )
					{
						if (realOnly == true)
							lastbool = ( resArrDou [resArrDouIndex-1] == resArrDou [resArrDouIndex] ) ? true : false;
						else
							lastbool = ( resArrInt [resArrIntIndex-1] == resArrInt [resArrIntIndex] ) ? true : false;
					}

					if 	((A1 -> data -> type == t_expr_int || A1 -> data -> type == sym_var_int) &&
						 (A2 -> data -> type == t_expr_dou || A2 -> data -> type == sym_var_rea)  )
					{
						if (realOnly == true)
							lastbool = ( resArrDou [resArrDouIndex-1] == resArrDou [resArrDouIndex] ) ? true : false;
						else
							lastbool = ( resArrInt [resArrIntIndex-1] == resArrInt [resArrIntIndex] ) ? true : false;
					}

					break;
				}

				errorHandler (errRunRest);

				break;
			
			case I_NEQUAL:
				
							
				A1 = (tNodePtr) new -> adr1;
				A2 = (tNodePtr) new -> adr2;

				// Prisli nam dve nove adresy => zaciatok noveho medzivypoctu

				if (A1 -> data -> used == false && A2 -> data -> used == false)
				{

					if (A1 -> data -> content . initialized == false ||
						A2 -> data -> content . initialized == false  ) 
						errorHandler (errRunUnin);

					
					if ((A1 -> data -> type == t_expr_int || A1 -> data -> type == sym_var_int) &&
						(A2 -> data -> type == t_expr_int || A2 -> data -> type == sym_var_int))
					{	
						lastbool = (A1 -> data -> content . integer != A2 -> data -> content . integer ) ? true : false;
					}

					if ((A1 -> data -> type == t_expr_dou || A1 -> data -> type == sym_var_rea) &&
						(A2 -> data -> type == t_expr_dou || A2 -> data -> type == sym_var_rea))
					{	
						lastbool = (A1 -> data -> content . real != A2 -> data -> content . real ) ? true : false;
					}

					if ((A1 -> data -> type == t_expr_str || A1 -> data -> type == sym_var_str) &&
						(A2 -> data -> type == t_expr_str || A2 -> data -> type == sym_var_str))
					{	
						cmp = strcmp (A1 -> data -> content . string, A2 -> data -> content . string);
						lastbool = (cmp != 0) ? true : false;
					}

					if ((A1 -> data -> type == t_expr_boo || A1 -> data -> type == sym_var_boo) &&
						(A2 -> data -> type == t_expr_boo || A2 -> data -> type == sym_var_boo))
					{	
						lastbool = (A1 -> data -> content . boolean != A2 -> data -> content . boolean) ? true : false;
					}
					A1 -> data -> used = true;
					A2 -> data -> used = true;

					break;
				}

				// Prva adresa nebola pouzita ale druha ano => pouzijem prvu

				if (A1 -> data -> used == false && A2 -> data -> used == true)
				{	

					if (A1 -> data -> content . initialized == false) 
						errorHandler (errRunUnin);

					if (A1 -> data -> type == t_expr_int || A1 -> data -> type == sym_var_int)
					{	
						lastbool = (A1 -> data -> content . integer != resArrInt [resArrIntIndex] ) ? true : false;
					}

					if (A1 -> data -> type == t_expr_dou || A1 -> data -> type == sym_var_rea)
					{	
						lastbool = (A1 -> data -> content . real != resArrDou [resArrDouIndex] ) ? true : false;
					}

					A1 -> data -> used = true;

					break;
				}

				// Prva adresa uz bola druha nie  => pouzijem druhu

				
				if (A1 -> data -> used == true && A2 -> data -> used == false)
				{	

					if (A2 -> data -> content . initialized == false) 
						errorHandler (errRunUnin);

					if (A2 -> data -> type == t_expr_int || A2 -> data -> type == sym_var_int)
					{	
						lastbool = ( resArrInt [resArrIntIndex] != A2 -> data -> content . integer ) ? true : false;
					}

					if (A2 -> data -> type == t_expr_dou || A2 -> data -> type == sym_var_rea)
					{	
						lastbool = ( resArrDou [resArrDouIndex] != A2 -> data -> content . real ) ? true : false;
					}

					A2 -> data -> used = true;

					break;
				}

				// Obe adresy boli pouzite => zratam medzivysledky

				if (A1 -> data -> used == true && A2 -> data -> used == true)
				{

					if 	((A1 -> data -> type == t_expr_int || A1 -> data -> type == sym_var_int) &&
						 (A2 -> data -> type == t_expr_int || A2 -> data -> type == sym_var_int)  )
					{
						if (realOnly == true)
							lastbool = ( resArrDou [resArrDouIndex-1] != resArrDou [resArrDouIndex] ) ? true : false;
						else
							lastbool = ( resArrInt [resArrIntIndex-1] != resArrInt [resArrIntIndex] ) ? true : false;
					}

					if 	((A1 -> data -> type == t_expr_dou || A1 -> data -> type == sym_var_rea) &&
						 (A2 -> data -> type == t_expr_dou || A2 -> data -> type == sym_var_rea)  )
					{
						lastbool = ( resArrDou [resArrDouIndex-1] != resArrDou [resArrDouIndex] ) ? true : false;
					}

					if 	((A1 -> data -> type == t_expr_dou || A1 -> data -> type == sym_var_rea) &&
						 (A2 -> data -> type == t_expr_int || A2 -> data -> type == sym_var_int)  )
					{
						if (realOnly == true)
							lastbool = ( resArrDou [resArrDouIndex-1] != resArrDou [resArrDouIndex] ) ? true : false;
						else
							lastbool = ( resArrInt [resArrIntIndex-1] != resArrInt [resArrIntIndex] ) ? true : false;
					}

					if 	((A1 -> data -> type == t_expr_int || A1 -> data -> type == sym_var_int) &&
						 (A2 -> data -> type == t_expr_dou || A2 -> data -> type == sym_var_rea)  )
					{
						if (realOnly == true)
							lastbool = ( resArrDou [resArrDouIndex-1] != resArrDou [resArrDouIndex] ) ? true : false;
						else
							lastbool = ( resArrInt [resArrIntIndex-1] != resArrInt [resArrIntIndex] ) ? true : false;
					}

					break;
				}

				errorHandler (errRunRest);

				break;
							//FUNKCIE//
			
			case I_READI:
				if(scanf("%d", &(((tData) new->result)->content.integer)) != 1)
					errorHandler(errRunLoad);
				((tData) new->result)->content.initialized = true;
				break;

			case I_READR:	
				if(scanf("%lg", &(((tData) new->result)->content.real)) != 1)
					errorHandler(errRunLoad);
				((tData) new->result)->content.initialized = true;
				break;	

			case I_READS:	
				if(scanf("%s", (((tData) new->result)->content.string)) != 1)
					errorHandler(errRunLoad);
				((tData) new->result)->content.initialized = true;
				break;

			case I_WRITE:
				
				for(int i = 0; i < *((int*) new->adr1); i++)
				{
					
					switch((*((tData**) new->adr2)[i])->type)
					{
						case sym_var_int:
						case t_expr_int:
							printf("%d", (*((tData**) new->adr2)[i])->content.integer);
							break;
					
						case sym_var_rea:
						case t_expr_dou:
							printf("%g", (*((tData**) new->adr2)[i])->content.real);
							break;
						
						case sym_var_str:
						case t_expr_str:
							printf("%s", (*((tData**) new->adr2)[i])->content.string);
							break;

						case sym_var_boo:
						case t_expr_boo:
							printf("%d", (*((tData**) new->adr2)[i])->content.boolean);		
						break;
					
						default: errorHandler(errInt);
								break;
					}
				}
				break;

			case I_IF:
		
				
				if(lastbool == true)	
				{
					tListItem * revert = currIL->active;
				
					recycleAdr = true;
					interpret(&rootTS, ((tInsList *) new->adr1));
					preklopenie(currIL);

					currIL->active = revert;
								
					break;
				} else 
				{
					tListItem * revert = currIL->active;
				
					recycleAdr = true;
					interpret(&rootTS, ((tInsList *) new->adr2));
					preklopenie(currIL);
					currIL->active = revert;
					break;
				}
			case I_VAR:break;
			
			case I_WHILE:

				while(lastbool == true)
				{
					recycleAdr = true;
					interpret(&rootTS, ((tInsList *) new->adr1));
					preklopenie(currIL);
				}
				break;

			case I_FCE:
					

				currentTerm = (((tData) new->adr1)->nextArg);
		
				if(new->result!=NULL)
				conVarOld = *((tContent *) new->result);
				
				for(int i = 0; i < ((tData) new->adr1)->argCount; i++)
				{
					conOld[i] = currentTerm->data->content;
                    currentTerm->data->content = (*((tContent**) new->adr2)[i]);
					currentTerm = currentTerm->data->nextArg;
				}

				tNodePtr hledam = searchSymbol  (&rootTS,"Flength");
				
				char *nazovfunkcie = ((tData) new->adr1)->name;
			
				if(strcmp(nazovfunkcie, "length") == 0)
				{
					readyInt = strlen(((tData) new->adr1)->nextArg->data->content.string);
				}
				else
					if(strcmp(nazovfunkcie, "copy") == 0)
					{
						laststring = funCopy(((tData) new->adr1)->nextArg->data->content.string,
											 ((tData) new->adr1)->nextArg->data->nextArg->data->content.integer,
											 ((tData) new->adr1)->nextArg->data->nextArg->data->nextArg->data->content.integer);
					}
					else
						if(strcmp(nazovfunkcie, "find") == 0)
						{
							readyInt = BMASearch(((tData) new->adr1)->nextArg->data->content.string,
											 ((tData) new->adr1)->nextArg->data->nextArg->data->content.string);
						}
						else
							if(strcmp(nazovfunkcie, "sort") == 0)
							{
								laststring = allocQuickSort(((tData) new->adr1)->nextArg->data->content.string,0,strlen(((tData) new->adr1)->nextArg->data->content.string)-1);
							}
							else
							{
								tListItem * revert = currIL->active;
								
								kanter++;
									
								recycleAdr = true;
								interpret(NULL, (((tData) new->adr1)->localILadr));
								preklopenie(currIL);
								
								
								
								currIL->active = revert;

                
            	
				                switch((((tData) new->adr1)->type))
								{
									case sym_fun_int:
									case sym_fok_int:	
										readyInt = (*((tContent *) new->result)).integer;
										break;

									case sym_fun_rea:
									case sym_fok_rea:
										lastdouble = (*((tContent *) new->result)).real;
										break;	

									case sym_fun_str:
									case sym_fok_str:
										laststring = (*((tContent *) new->result)).string;
										break;

									case sym_fun_boo:
									case sym_fok_boo:
										lastbool =(*((tContent *) new->result)).boolean;
										break;
									default: errorHandler(errInt); 
											break;
								}
			
				                *((tContent *) new->result) = conVarOld;
				            
				                currentTerm = (((tData) new->adr1)->nextArg);

								for(int i = 0; i < ((tData) new->adr1)->argCount; i++)
								{
									currentTerm->data->content = conOld[i];
									(((tContent**) new->adr2)[i])=NULL;
									currentTerm = currentTerm->data->nextArg;
								}
               				}

                

                	
			break;
			
			default: errorHandler(errInt);
					break;
		}	
		Succ(currIL);


	} while(currIL->active != NULL);
 return 0; 
}
