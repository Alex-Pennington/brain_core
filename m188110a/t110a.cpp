/*
 * General table files.
 *
 */

#include "Cm110s.h"

/* Transmit Filter */
/* 0.5 ALPHA raised cosine */

#define T00  -0.000378251609645f
#define T01  0.00200981026023f
#define T02  0.0008118277103f
#define T03  5.06151431829e-05f
#define T04  0.000468938964905f
#define T05  0.00300280854211f
#define T06  0.0058455196582f
#define T07  0.00409430858276f
#define T08  -0.00641287983193f
#define T09  -0.0230667932848f
#define T10  -0.0334472346019f
#define T11  -0.0199033035076f
#define T12  0.0289866748659f
#define T13  0.107263161264f
#define T14  0.189692575735f
#define T15  0.242732784244f
#define T16  0.242732784244f
#define T17  0.189692575735f
#define T18  0.107263161264f
#define T19  0.0289866748659f
#define T20  -0.0199033035076f
#define T21 -0.0334472346019f
#define T22  -0.0230667932848f
#define T23  -0.00641287983193f
#define T24  0.00409430858276f
#define T25  0.0058455196582f
#define T26  0.00300280854211f
#define T27  0.000468938964905f
#define T28  5.06151431829e-05f
#define T29  0.0008118277103f
#define T30  0.00200981026023f
#define T31  -0.000378251609645f

/* Transmit Filter 0.2 ALPHA */
float tx_coffs[M1_TX_FILTER_LENGTH]=
{
	(T00+T01+T02+T03),
	(T00+T01+T02),
	(T00+T01),
	(T00),
	(T04+T05+T06+T07),
	(T03+T04+T05+T06),
	(T02+T03+T04+T05),
	(T01+T02+T03+T04),
	(T08+T09+T10+T11),
	(T07+T08+T09+T10),
	(T06+T07+T08+T09),
	(T05+T06+T07+T08),
	(T12+T13+T14+T15),
	(T11+T12+T13+T14),
	(T10+T11+T12+T13),
	(T09+T10+T11+T12),
	(T16+T17+T18+T19),
	(T15+T16+T17+T18),
	(T14+T15+T16+T17),
	(T13+T14+T15+T16),
	(T20+T21+T22+T23),
	(T19+T20+T21+T22),
	(T18+T19+T20+T21),
	(T17+T18+T19+T20),
	(T24+T25+T26+T27),
	(T23+T24+T25+T26),
	(T22+T23+T24+T25),
	(T21+T22+T23+T24),
	(T28+T29+T30+T31),
	(T27+T28+T29+T30),
	(T26+T27+T28+T29),
	(T25+T26+T27+T28),
};

/* 16 tap LPF to prevent aliasing hamming window  */
/* Main filtering relies on the radios IF filters.*/

float rx_coffs[M1_RX_FILTER_LENGTH]=
{
	 0.001572f,
     0.004287f,
     0.004740f,
    -0.006294f,
    -0.028729f,
    -0.034880f,
     0.015939f,
     0.131216f,
     0.257323f,
     0.312500f,
     0.257323f,
     0.131216f,
     0.015939f,
    -0.034880f,
    -0.028729f,
    -0.006294f,
     0.004740f,
     0.004287f,
     0.001572f 
};
/* Transmit constellation */
FComplex con_symbol[8]=
{
	{ 1.000f, 0.000f},
	{ 0.707f, 0.707f},
	{ 0.000f, 1.000f},
	{-0.707f, 0.707f},
	{-1.000f, 0.000f},
	{-0.707f,-0.707f},
	{ 0.000f,-1.000f},
	{ 0.707f,-0.707f}
};
/* tx symbols used to represent the preamble symbols */
int  psymbol[8][32]=
{
	{0,0,0,0,0,0,0,0},
	{0,4,0,4,0,4,0,4},
	{0,0,4,4,0,0,4,4},
	{0,4,4,0,0,4,4,0},
	{0,0,0,0,4,4,4,4},
	{0,4,0,4,4,0,4,0},
	{0,0,4,4,4,4,0,0},
	{0,4,4,0,4,0,0,4}
};
/* sequence used to scramble the preamble */
int pscramble[32]=
{
	7,4,3,0,5,1,5,0,2,2,1,1,5,7,4,3,5,0,2,6,2,1,6,2,0,0,5,0,5,2,6,6
};
int p_c_seq[9]=
{
	0,1,3,0,1,3,1,2,0
};
/* Modified Gray Codes */
int mgd3[8] =
{
	0,1,3,2,7,6,4,5
};
int mgd2[4] =
{
	0,1,3,2
};
/* 75 bps symbol mapping */
int mns[4][32] =
{
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,4,0,4,0,4,0,4,0,4,0,4,0,4,0,4,0,4,0,4,0,4,0,4,0,4,0,4,0,4,0,4},
	{0,0,4,4,0,0,4,4,0,0,4,4,0,0,4,4,0,0,4,4,0,0,4,4,0,0,4,4,0,0,4,4},
	{0,4,4,0,0,4,4,0,0,4,4,0,0,4,4,0,0,4,4,0,0,4,4,0,0,4,4,0,0,4,4,0}
};
int mes[4][32] =
{
	{0,0,0,0,4,4,4,4,0,0,0,0,4,4,4,4,0,0,0,0,4,4,4,4,0,0,0,0,4,4,4,4},
	{0,4,0,4,4,0,4,0,0,4,0,4,4,0,4,0,0,4,0,4,4,0,4,0,0,4,0,4,4,0,4,0},
	{0,0,4,4,4,4,0,0,0,0,4,4,4,4,0,0,0,0,4,4,4,4,0,0,0,0,4,4,4,4,0,0},
	{0,4,4,0,4,0,0,4,0,4,4,0,4,0,0,4,0,4,4,0,4,0,0,4,0,4,4,0,4,0,0,4}
};
int psk8[8] = 
{
	0,1,2,3,4,5,6,7
};
int psk4[4] = 
{
	0,2,4,6
};
int psk2[8] = 
{
	0,4
};


/*
 *
 * Memory used for interleaver and de-interleaver.
 *
 */

void Cm110s:: create_data_scrambler_seq( void )
{
	int sreg[12];
	int carry,i,j;
	
	sreg[0]  = 1;
	sreg[1]  = 0;
	sreg[2]  = 1;
	sreg[3]  = 1;
	sreg[4]  = 0;
	sreg[5]  = 1;
	sreg[6]  = 0;
	sreg[7]  = 1;
	sreg[8]  = 1;
	sreg[9]  = 1;
	sreg[10] = 0;
	sreg[11] = 1;

	for( i = 0; i < M1_DATA_SCRAMBLER_LENGTH; i++ )
	{			
		for( j = 0; j < 8; j++ )
		{
			carry    = sreg[11];
			sreg[11] = sreg[10];
			sreg[10] = sreg[9];
			sreg[9]  = sreg[8];
			sreg[8]  = sreg[7];
			sreg[7]  = sreg[6];
			sreg[6]  = sreg[5]^carry;
			sreg[5]  = sreg[4];
			sreg[4]  = sreg[3]^carry;
			sreg[3]  = sreg[2];
			sreg[2]  = sreg[1];
			sreg[1]  = sreg[0]^carry;
			sreg[0]  = carry;			
		}
		data_scrambler_bits[i]  = (sreg[2]<<2)+(sreg[1]<<1)+(sreg[0]);	
		data_scrambler_seq[i]   = con_symbol[data_scrambler_bits[i]];
	}
}
void Cm110s::create_mes_and_mns_seq( void )
{
	int i,j;
	
	for( i = 0; i < 4; i++ )
	{
		for( j = 0; j < 32; j++ )
		{
			mes_seq[i][j] = con_symbol[mes[i][j]]; 	
			mns_seq[i][j] = con_symbol[mns[i][j]];	
		}
	}
}
void Cm110s::create_prx_tables( void )
{
	int i,j;

	for( i = 0; i < 8; i++ )
	{
		for( j = 0; j < 32; j++ )
		{
			prx[i][j] = con_symbol[(pscramble[j]+psymbol[i][j])%8];
		}
	}
}