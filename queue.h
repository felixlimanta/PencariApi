/* File : queue.h */
/* Definisi ADT Queue dengan representasi array secara eksplisit dan alokasi dinamik */
/* Model Implementasi Versi III dengan circular buffer */

#ifndef queue_H
#define queue_H

#include "boolean.h"

#define Nil 0
#define MaxEl 20
/* Konstanta untuk mendefinisikan address tak terdefinisi */

/* Definisi elemen dan address */
typedef struct {
	int num;
	int degree;
} node;

typedef int address;   /* indeks tabel */
/* Contoh deklarasi variabel bertype Queue : */
/* Versi I : tabel dinamik, Head dan Tail eksplisit, ukuran disimpan */
typedef struct {
	node T[MaxEl+1]; /* tabel penyimpan elemen */
	address HEAD;  /* alamat TOP: elemen puncak */
	address TAIL;
} Queue;
/* Definisi Queue kosong: HEAD=Nil; TAIL=Nil. */
/* Catatan implementasi: T[0] tidak pernah dipakai */

/* ********* AKSES (Selektor) ********* */
/* Jika Q adalah Queue, maka akses elemen : */
#define Head(Q) (Q).HEAD
#define Tail(Q) (Q).TAIL
#define InfoHead(Q) (Q).T[(Q).HEAD]
#define InfoTail(Q) (Q).T[(Q).TAIL]

boolean IsEmpty (Queue Q)
/* Mengirim true jika Q kosong: lihat definisi di atas */
{
    return (Head(Q)==Nil && Tail(Q)==Nil);
}

boolean IsFull (Queue Q)
/* Mengirim true jika tabel penampung elemen Q sudah penuh */
/* yaitu mengandung elemen sebanyak MaxEl */
{
    // ALGORITMA
    if (Tail(Q)==MaxEl) {
        return (Head(Q)==1);
    }
    else {
        return (Head(Q)==(Tail(Q)+1));
    }
}

int NBElmt (Queue Q)
/* Mengirimkan banyaknya elemen queue. Mengirimkan 0 jika Q kosong. */
{
    // ALGORITMA
    if (IsEmpty(Q)) {
        return 0;
    }
    else {
        if (Tail(Q) > Head(Q)) {
            return (Tail(Q)-Head(Q)+1);
        }
        else if (Head(Q)==Tail(Q)) {
            return 1;
        }
        else { // Head(Q) > Tail(Q)
            return (MaxEl-Head(Q)+Tail(Q)+1);
        }
    }
}

/* *** Kreator *** */
void CreateEmpty (Queue * Q)
/* I.S. sembarang */
/* F.S. Sebuah Q kosong terbentuk dan salah satu kondisi sbb: */
/* Jika alokasi berhasil, Tabel memori dialokasi berukuran Max+1 */
/* atau : jika alokasi gagal, Q kosong dg MaxEl=0 */
/* Proses : Melakukan alokasi, membuat sebuah Q kosong */
{
    // ALGORITMA
    Head(*Q) = Nil;
    Tail(*Q) = Nil;

}

/* *** Primitif Add/Delete *** */
void Add (Queue * Q, node X)
/* Proses: Menambahkan X pada Q dengan aturan FIFO */
/* I.S. Q mungkin kosong, tabel penampung elemen Q TIDAK penuh */
/* F.S. X menjadi TAIL yang baru, TAIL "maju" dengan mekanisme circular buffer */
{
    // ALGORITMA
    if (IsEmpty(*Q)) {
        Head(*Q) = 1;
    }
    else {
        if (Tail(*Q)==MaxEl) {
            Tail(*Q) -= MaxEl;
        }
    }
    Tail(*Q)++;
    InfoTail(*Q) = X;
}

void Del (Queue * Q, node * X)
/* Proses: Menghapus X pada Q dengan aturan FIFO */
/* I.S. Q tidak mungkin kosong */
/* F.S. X = nilai elemen HEAD pd I.S., HEAD "maju" dengan mekanisme circular buffer;
        Q mungkin kosong */
{
    // ALGORITMA
    *X = InfoHead(*Q);
    if (NBElmt(*Q)==1) {
        Head(*Q) = Nil;
        Tail(*Q) = Nil;
    }
    else {
        if (Head(*Q)==MaxEl) {
            Head(*Q)-=MaxEl;
        }
        Head(*Q)++;
    }
}
#endif
