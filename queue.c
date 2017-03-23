//////////////////////////////////////////////////////
//                                                  //
//    NIM       : 13515077                          //
//    Nama      : Rionaldi Chandraseta              //
//    Tanggal   : 29 September 2016                 //
//    Topik     : Queue                             //
//    Deskripsi : File body struktur data queue     //
//                                                  //
//////////////////////////////////////////////////////

/* Model Implementasi Versi III dengan circular buffer */

#include "boolean.h"
#include "queue.h"
#include <stdlib.h>

/* ********* Prototype ********* */
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
    if (Tail(Q)==MaxEl(Q)) {
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
            return (MaxEl(Q)-Head(Q)+Tail(Q)+1);
        }
    }
}

/* *** Kreator *** */
void CreateEmpty (Queue * Q, int Max)
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
void Add (Queue * Q, infotype X)
/* Proses: Menambahkan X pada Q dengan aturan FIFO */
/* I.S. Q mungkin kosong, tabel penampung elemen Q TIDAK penuh */
/* F.S. X menjadi TAIL yang baru, TAIL "maju" dengan mekanisme circular buffer */
{
    // ALGORITMA
    if (IsEmpty(*Q)) {
        Head(*Q) = 1;
    }
    else {
        if (Tail(*Q)==MaxEl(*Q)) {
            Tail(*Q) -= MaxEl(*Q);
        }
    }
    Tail(*Q)++;
    InfoTail(*Q) = X;
}

void Del (Queue * Q, infotype * X)
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
        if (Head(*Q)==MaxEl(*Q)) {
            Head(*Q)-=MaxEl(*Q);
        }
        Head(*Q)++;
    }
}