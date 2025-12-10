/*
 *
 * Interleaver/Deinterleaver for MIL-STD 188-110A Serial tone modem.
 *
 *
 * 14th July 2001
 *
 * C.H Brain
 *
 */
#include "Cm110s.h"

int Cm110s::calculate_interleaver_size( Interleaver *lvr )
{
	return(lvr->row_nr*lvr->col_nr);
}
int Cm110s::calculate_deinterleaver_size( DeInterleaver *lvr )
{
	return(lvr->row_nr*lvr->col_nr);
}
void Cm110s::reset_interleave( Interleaver *lvr )
{
	lvr->row      = 0;
	lvr->col      = 0;
	lvr->zrow     = 0;
	lvr->zcol     = 0;
	lvr->col_last = 0;
	lvr->nbits    = 0;
}
void Cm110s::load_interleaver( int bit, Interleaver *lvr )
{
	if( lvr->row_inc != 0 )
	{
		lvr->array[lvr->row][lvr->col] = bit;
		lvr->row = (lvr->row+lvr->row_inc)%lvr->row_nr;
		if( lvr->row == 0 )
		{
			lvr->col = (lvr->col+1)%lvr->col_nr;
		}
	}
	else
	{
		lvr->array[lvr->row][lvr->col] = bit;
		lvr->row = (lvr->row+1)%lvr->row_nr;
		if( lvr->row == 0 )
		{
			lvr->col = (lvr->col+1)%lvr->col_nr;
		}
	}
	lvr->nbits++;
}
int Cm110s::fetch_interleaver( Interleaver *lvr )
{
	int bit;

	if( lvr->row_inc != 0 )
	{
		bit = lvr->array[lvr->row][lvr->col];
		lvr->row = (lvr->row+1)%lvr->row_nr;
		lvr->col = (lvr->col+lvr->col_inc)%lvr->col_nr;
		if( lvr->row == 0 )
		{
			lvr->col      = (lvr->col_last+1)%lvr->col_nr;
			lvr->col_last = lvr->col;
		}
	}
	else
	{
		bit = lvr->array[lvr->zrow][lvr->zcol];
		lvr->zrow = (lvr->zrow+1)%lvr->row_nr;
		if( lvr->zrow == 0 )
		{
			lvr->zcol = (lvr->zcol+1)%lvr->col_nr;
		}
	}
	lvr->nbits--;
	return bit;
}
void Cm110s::load_deinterleaver( float bit, DeInterleaver *lvr )
{
	if( lvr->row_inc != 0 )
	{
		lvr->array[lvr->row][lvr->col] = bit;
		lvr->row = (lvr->row+1)%lvr->row_nr;
		lvr->col = (lvr->col+lvr->col_inc)%lvr->col_nr;

		if( lvr->row == 0 )
		{
			lvr->col      = (lvr->col_last+1)%lvr->col_nr;
			lvr->col_last = lvr->col;
		}
	}
	else
	{
		lvr->array[lvr->row][lvr->col] = bit;
		lvr->row = (lvr->row+1)%lvr->row_nr;
		if( lvr->row == 0 )
		{
			lvr->col = (lvr->col+1)%lvr->col_nr;
		}
	}
	lvr->nbits++;
}
float Cm110s::fetch_deinterleaver( DeInterleaver *lvr )
{
	float bit;

	if( lvr->row_inc != 0 )
	{
		bit = lvr->array[lvr->row][lvr->col];
		lvr->row = (lvr->row+lvr->row_inc)%lvr->row_nr;

		if( lvr->row == 0 )
		{
			lvr->col = (lvr->col+1)%lvr->col_nr;
		}
	}
	else
	{
		bit = lvr->array[lvr->zrow][lvr->zcol];
		lvr->zrow = (lvr->zrow+1)%lvr->row_nr;
		if( lvr->zrow == 0 )
		{
			lvr->zcol = (lvr->zcol+1)%lvr->col_nr;
		}
	
	}
	lvr->nbits--;
	return bit;
}
void Cm110s::reset_deinterleave( DeInterleaver *lvr )
{
	lvr->row      = 0;
	lvr->col      = 0;
	lvr->col_last = 0;
	lvr->nbits    = 0;
}
void Cm110s::clear_deinterleave( DeInterleaver *lvr )
{
	int i,j;
	
	for( i=0; i< lvr->row_nr; i++ )
	{
		for( j=0; j < lvr->col_nr; j++ )
		{
			lvr->array[i][j] = 0.0;
		}	
	}
}
//
// Also sets the known data modulus.
//
void Cm110s::set_interleave_mode( Mode mode, Interleaver *lvr )
{

	// ADDED Test
	rx_known_errors = 0;

	reset_interleave( lvr );
		
	switch(mode)
	{
		case M75NS:
			lvr->row_nr  = 10;
			lvr->col_nr  = 9;
			lvr->row_inc = 7;
			lvr->col_inc = lvr->col_nr - 7;
			tx_block_count_mod = 45;
			break;
		case M75NL:
			lvr->row_nr  = 20;
			lvr->col_nr  = 36;
			lvr->row_inc = 7;
			lvr->col_inc = lvr->col_nr - 7;
			tx_block_count_mod = 360;
			break;
		case M150S:
			lvr->row_nr  = 40;
			lvr->col_nr  = 18;
			lvr->row_inc = 9;
			lvr->col_inc = lvr->col_nr - 17;
			tx_block_count_mod = 36;
			break;
		case M150L:
			lvr->row_nr  = 40;
			lvr->col_nr  = 144;
			lvr->row_inc = 9;
			lvr->col_inc = lvr->col_nr - 17;
			tx_block_count_mod = 288;
			break;
		case M300S:
			lvr->row_nr  = 40;
			lvr->col_nr  = 18;
			lvr->row_inc = 9;
			lvr->col_inc = lvr->col_nr - 17;
			tx_block_count_mod = 36;
			break;
		case M300L:
			lvr->row_nr  = 40;
			lvr->col_nr  = 144;
			lvr->row_inc = 9;
			lvr->col_inc = lvr->col_nr - 17;
			tx_block_count_mod = 288;
			break;
		case M600S:
			lvr->row_nr  = 40;
			lvr->col_nr  = 18;
			lvr->row_inc = 9;
			lvr->col_inc = lvr->col_nr - 17;
			tx_block_count_mod = 36;
			break;
		case M600L:
			lvr->row_nr  = 40;
			lvr->col_nr  = 144;
			lvr->row_inc = 9;
			lvr->col_inc = lvr->col_nr - 17;
			tx_block_count_mod = 288;
			break;
		case M600V:
			lvr->row_nr = 40;
			lvr->col_nr = 18;
			lvr->row_inc = 9;
			lvr->col_inc = lvr->col_nr - 17;
			tx_block_count_mod = 36;
			break;
		case M1200S:
			lvr->row_nr  = 40;
			lvr->col_nr  = 36;
			lvr->row_inc = 9;
			lvr->col_inc = lvr->col_nr - 17;
			tx_block_count_mod = 36;
			break;
		case M1200L:
			lvr->row_nr  = 40;
			lvr->col_nr  = 288;
			lvr->row_inc = 9;
			lvr->col_inc = lvr->col_nr - 17;
			tx_block_count_mod = 288;
			break;
		case M1200V:
			lvr->row_nr = 40;
			lvr->col_nr = 36;
			lvr->row_inc = 9;
			lvr->col_inc = lvr->col_nr - 17;
			tx_block_count_mod = 36;
			break;
		case M2400S:
			lvr->row_nr  = 40;
			lvr->col_nr  = 72;
			lvr->row_inc = 9;
			lvr->col_inc = lvr->col_nr - 17;
			tx_block_count_mod = 30;
			break;
		case M2400L:
			lvr->row_nr  = 40;
			lvr->col_nr  = 576;
			lvr->row_inc = 9;
			lvr->col_inc = lvr->col_nr - 17;
			tx_block_count_mod = 240;
			break;
		case M2400V:
			lvr->row_nr  = 40;
			lvr->col_nr  = 72;
			lvr->row_inc = 0;
			lvr->col_inc = 0;
			tx_block_count_mod = 30;
			break;
		case M4800S:
			lvr->row_nr  = 40;
			lvr->col_nr  = 72;
			lvr->row_inc = 0;
			lvr->col_inc = 0;
			tx_block_count_mod = 30;
			break;
		default:
			break;
	}
	reset_interleave( lvr );
	lvr->mode = mode;
	lvr->size = calculate_interleaver_size( lvr );
}
//
// Also sets the known data modulus.
//
void Cm110s::set_de_interleave_mode( Mode mode, DeInterleaver *lvr )
{

	reset_deinterleave( lvr );
		
	switch(mode)
	{
		case M75NS:
			lvr->row_nr  = 10;
			lvr->col_nr  = 9;
			lvr->row_inc = 7;
			lvr->col_inc = lvr->col_nr - 7;
			rx_block_count_mod = 45;
			break;
		case M75NL:
			lvr->row_nr  = 20;
			lvr->col_nr  = 36;
			lvr->row_inc = 7;
			lvr->col_inc = lvr->col_nr - 7;
			rx_block_count_mod = 360;
			break;
		case M150S:
			lvr->row_nr  = 40;
			lvr->col_nr  = 18;
			lvr->row_inc = 9;
			lvr->col_inc = lvr->col_nr - 17;
			rx_block_count_mod = 36;
			break;
		case M150L:
			lvr->row_nr  = 40;
			lvr->col_nr  = 144;
			lvr->row_inc = 9;
			lvr->col_inc = lvr->col_nr - 17;
			rx_block_count_mod = 288;
			break;
		case M300S:
			lvr->row_nr  = 40;
			lvr->col_nr  = 18;
			lvr->row_inc = 9;
			lvr->col_inc = lvr->col_nr - 17;
			rx_block_count_mod = 36;
			break;
		case M300L:
			lvr->row_nr  = 40;
			lvr->col_nr  = 144;
			lvr->row_inc = 9;
			lvr->col_inc = lvr->col_nr - 17;
			rx_block_count_mod = 288;
			break;
		case M600S:
			lvr->row_nr  = 40;
			lvr->col_nr  = 18;
			lvr->row_inc = 9;
			lvr->col_inc = lvr->col_nr - 17;
			rx_block_count_mod = 36;
			break;
		case M600L:
			lvr->row_nr  = 40;
			lvr->col_nr  = 144;
			lvr->row_inc = 9;
			lvr->col_inc = lvr->col_nr - 17;
			rx_block_count_mod = 288;
			break;
		case M600V:
			lvr->row_nr = 40;
			lvr->col_nr = 18;
			lvr->row_inc = 9;
			lvr->col_inc = lvr->col_nr - 17;
			rx_block_count_mod = 36;
			break;
		case M1200S:
			lvr->row_nr  = 40;
			lvr->col_nr  = 36;
			lvr->row_inc = 9;
			lvr->col_inc = lvr->col_nr - 17;
			rx_block_count_mod = 36;
			break;
		case M1200L:
			lvr->row_nr  = 40;
			lvr->col_nr  = 288;
			lvr->row_inc = 9;
			lvr->col_inc = lvr->col_nr - 17;
			rx_block_count_mod = 288;
			break;
		case M1200V:
			lvr->row_nr = 40;
			lvr->col_nr = 36;
			lvr->row_inc = 9;
			lvr->col_inc = lvr->col_nr - 17;
			rx_block_count_mod = 36;
			break;
		case M2400S:
			lvr->row_nr  = 40;
			lvr->col_nr  = 72;
			lvr->row_inc = 9;
			lvr->col_inc = lvr->col_nr - 17;
			rx_block_count_mod = 30;
			break;
		case M2400L:
			lvr->row_nr  = 40;
			lvr->col_nr  = 576;
			lvr->row_inc = 9;
			lvr->col_inc = lvr->col_nr - 17;
			rx_block_count_mod = 240;
			break;
		case M2400V:
			lvr->row_nr  = 40;
			lvr->col_nr  = 72;
			lvr->row_inc = 0;
			lvr->col_inc = 0;
			rx_block_count_mod = 30;
			break;
		case M4800S:
			lvr->row_nr  = 40;
			lvr->col_nr  = 72;
			lvr->row_inc = 0;
			lvr->col_inc = 0;
			rx_block_count_mod = 30;
			break;
		default:
			break;
	}
	reset_deinterleave( lvr );
	lvr->mode = mode;
	lvr->size = calculate_deinterleaver_size( lvr );
	clear_deinterleave( lvr );
}