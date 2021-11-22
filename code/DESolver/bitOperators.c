/**
 * MaxSAT solver based on Differential Evolution
 * Copyright (C) 2021  Manuel Framil de Amorin (m.framil.deamorin@udc.es)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
**/

#define BitsInt             sizeof(int)
#define IntToBit(v)         ( v ? 1 : 0)
#define SetBit(A,k)         ( A[(k/BitsInt)] |= (1 << (k%BitsInt)) )
#define ClearBit(A,k)       ( A[(k/BitsInt)] &= ~(1 << (k%BitsInt)) )
#define GetBit(A,k)         (IntToBit( A[(k/BitsInt)] & (1 << (k%BitsInt)) ))
#define InvertBit(A,k)      ( A[(k/BitsInt)] ^= (1 << (k%BitsInt)) )
#define AssignBit(A,k,v)   ( v ? SetBit(A,k) : ClearBit(A,k))
