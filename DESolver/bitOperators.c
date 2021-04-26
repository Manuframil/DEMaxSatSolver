
#define IntToBit(v)         ( v ? 1 : 0)
#define SetBit(A,k)         ( A[(k/32)] |= (1 << (k%32)) )
#define ClearBit(A,k)       ( A[(k/32)] &= ~(1 << (k%32)) )
#define GetBit(A,k)         (IntToBit( A[(k/32)] & (1 << (k%32)) ))
#define InvertBit(A,k)      ( A[(k/32)] ^= (1 << (k%32)) ) 
#define AssignBit(A,k,v)   ( v ? SetBit(A,k) : ClearBit(A,k))
