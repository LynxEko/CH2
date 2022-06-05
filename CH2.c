#include <stdio.h>
#include <stdlib.h>

// Variable used to generate pseudo-random numbers
unsigned int seed;

// Function to generate pseudo-random numbers
unsigned int myRandom() {
  seed = (214013*seed+2531011);
  return (seed>>13);
}

void GenerateBOARD ( unsigned short BOARD[], int DIM, unsigned short MAX_VAL )
{
  // Generation of random values is different from CH1: this version is not really equivalent

  // Set default value as zero
  for (int idx=0; idx<DIM*DIM; idx++)
    BOARD[idx] = 0;

  // Generate random values in random positions
  for (int idx=0; idx<DIM*DIM; idx++)
    BOARD[idx] = myRandom() % MAX_VAL;
}


void CopyBOARD ( unsigned short IN[], unsigned short OUT[], int D )
{
  for (int y=1; y<D-1; y++)
    for (int x=1; x<D-1; x++)
      OUT[x+D*y] = IN[x+D*y];
}


void __attribute__ ((noinline)) 
  UpdateBOARD ( unsigned short IN[], unsigned short OUT[], int D, unsigned short MAX_VAL )
{
  unsigned short max1, max2, min1, min2, a, b, c, d, v;
  for (int y=1; y<D-1; y++)
    for (int x=1; x<D-1; x++) // access consecutive elements in inner loop
    {
      // copy values of neighbour elements
      a= IN[x+1+D*y];
      b= IN[x-1+D*y];
      c= IN[x+D*(y-1)];
      d= IN[x+D*(y+1)];

      // Sort using Sorting Network
      max1 = a>b? a: b;
      min1 = a>b? b: a;
      max2 = c>d? c: d;
      min2 = c>d? d: c;

      a    = min1>min2? min2: min1;
      b    = min1>min2? min1: min2;
      c    = max1>max2? max2: max1;
      d    = max1>max2? max1: max2;

      v = (b+c) % MAX_VAL;
      OUT[x+D*y] = v;
    }
}


void PrintBOARD ( unsigned short BOARD[], int D )
{
  for (int x=0; x<D; x++) {
    for (int y=0; y<D; y++)
      printf("%4d ",BOARD[x+D*y]);
    printf("\n");
  }
}


void PrintCHECK ( unsigned short BOARD[], int D )
{
  // Line x=1:
  for (int y=0; y<20; y++)
    printf("%4d ",BOARD[1+D*y]);
  printf("\n");

  // Line x=2:
  for (int y=0; y<20; y++)
    printf("%4d ",BOARD[2+D*y]);
  printf("\n");

  // Line x=D-2:
  for (int y=0; y<20; y++)
    printf("%4d ",BOARD[D-2+D*y]);
  printf("\n");  
}


void CopyBoundary ( unsigned short IN[], unsigned short OUT[], int D )
{
  // Up and Down Horizontal Boundaries
  for (int x=0; x<D; x++)
  { // y=0 and y=D-1
    OUT[x] = IN[x];
    OUT[x+D*(D-1)] = IN[x+D*(D-1)];
  }

  // Left and Rigth Vertical Boundaries
  for (int y=1; y<D-1; y++)
  { // x=0 and x=D-1
    OUT[y*D] = IN[y*D];
    OUT[y*D+D-1] = IN[y*D+D-1];
  }
}


void __attribute__ ((noinline)) 
  FillHistogram ( unsigned short BOARD[], unsigned Freq[], unsigned LocalId[], int D, int ValMax )
{
  // clear histogram
  for (int i=0; i<ValMax; i++)
    Freq[i] = 0;

  for (int x=0; x<D; x++)
    for (int y=0; y<D; y++)
    {
      unsigned short V = BOARD[x+D*y];
      unsigned Id      = Freq[V];
      Freq[V]          = Id+1;
      LocalId[x+y*D]   = Id;
    }
}

void PrefixSum ( unsigned Freq[], int ValMax )
{
  unsigned V = Freq[0], Sum = 0;
  Freq[0] = Sum;
  for (int i=1; i<ValMax; i++)
  {
    Sum = Sum + V;
    V   = Freq[i]; 
    Freq[i] = Sum;
  }
}


void PrintPrefix ( unsigned Freq[], int ValMax )
{
  for (int i=0; i<ValMax; i++)
    printf("%4d:%5d ", i, Freq[i]);
  printf("\n");
}


unsigned BinSearch( unsigned Vector[], int N, unsigned target )
{
  unsigned M, L= 0, R= N;
  M = (R - L)/2;
  do {
     M = M + L; 
     unsigned value = Vector[M];
     if (value < target)  L=M;  else  R=M;
     M = (R - L)/2;      
  } while (M);    
  return R;
}


void __attribute__ ((noinline)) 
  UpdateReversed ( unsigned short BOARD[], unsigned Freq[], unsigned LocalId[], int D, int ValMax )
{
  for (int xy=0; xy<D*D; xy++)
  {
    unsigned short V = BOARD[xy];
    unsigned pos     = Freq[V]+LocalId[xy];
    BOARD[xy]        = BinSearch( Freq, ValMax, D*D-pos)-1;
  }
}


int main (int argc, char **argv)
{
  int N=10, D=4002, Iter=50, MAX= 9999;
  seed = 12345;

  // obtain parameters at run time
  if (argc>1) { D   = atoi(argv[1]); }
  if (argc>2) { N   = atoi(argv[2]); }
  if (argc>3) { Iter= atoi(argv[3]); }

  printf("Challenge #2: DIM= %d, N= %d, Iter= %d\n", D, N, Iter);

  unsigned short *BOARD, *TMP;
  unsigned       *Freq, *LocID;

  BOARD= (unsigned short *) malloc( D*D* sizeof(unsigned short) );
  TMP  = (unsigned short *) malloc( D*D* sizeof(unsigned short) );
  Freq = (unsigned *)       malloc( MAX* sizeof(unsigned) );
  LocID= (unsigned *)       malloc( D*D* sizeof(unsigned) );

  GenerateBOARD ( BOARD, D, MAX );

  for (int i=0; i<N; i++)
  {
    for (int r=0; r<Iter; r++)
    {
      UpdateBOARD  ( BOARD, TMP, D, MAX );
      CopyBOARD    ( TMP, BOARD, D );
    }

    FillHistogram  ( BOARD, Freq, LocID, D, MAX );
    PrefixSum      ( Freq, MAX );
    UpdateReversed ( BOARD, Freq, LocID, D, MAX );
  }

  PrintCHECK ( BOARD, D );

  return 0;
}
