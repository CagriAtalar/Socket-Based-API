Socket Tabanlı Opcode Client–Server Mimarisi 🎉

Lesgoooo :D !  Bu proje, client ve server arasında kendi özel protokolümüzle iletişim kurmamızı sağlıyor. Her socket geldiğinde server yeni bir thread açıyor ve progress() fonksiyonuna gönderiyor. Burada mesaj decode ediliyor, opcode’a göre işlem yapılıyor ve sonuç client’a TRAP_TABLE üzerinden dönüyor. Tadaaa :D Mutex kullandım ki dosyaya birden fazla client geldiğinde sıkıntı olmasın. 

 Protokol – Nasıl Konuşuyoruz?

Mesaj formatımız şöyle:

<LEN>#<OPCODE>[<LEN>#<ARG>...]

LEN → ilgili kısmın byte uzunluğu

OPCODE → ne yapacağımızı belirten komut

ARG → OPCODE’e bağlı parametre(ler)

Örnekler

4#push4#elma → “push” komutu, argüman = “elma” (4 byte)

3#pop → son eklenen kelimeyi pop et

Bu şekilde encode/decode etmeyi de neetcode'da 1 soruyu çözerken gördüm ve baya hoşuma gitti. Ben de opcode ve argüman ayrımını tek mesajda bu şekilde yapmak istedim.

🔧 Opcode Enum – Bizim Mini REST API 
typedef enum {
    OP_PUSH,
    OP_POP,
    OP_UNKNOWN,
} OPCODE;


Ne işe yarıyorlar? Lesgo! 

PUSH → Argümanı DATABASE.txt’e ekle

POP → Son eklenen kelimeyi çıkar


 TRAP_TABLE – Hata/OK Durumları

Server’dan gelen cevaplar TRAP_TABLE üzerinden:

typedef enum {
    TRAP_OK = 0,
    TRAP_INVALID_OPCODE = 1,
    TRAP_FILE_ERROR = 2,
    TRAP_ARG_ERROR = 3,
    TRAP_UNKNOWN_ERROR = 99
} TRAP_TABLE;


Client alıyor, bakıyor:

TRAP:0 → Başarılı 

TRAP:2 → Dosya hatası 

Çoklu Thread – Multithread!

accept() ile gelen her client için yeni thread açılıyor

Thread progress()’e gidiyor

decode_message() ile opcode ve argümanlar ayrıştırılıyor

Mutex ile DATABASE.txt güvenli şekilde yazılıyor

İşlem bitince socket kapanıyor, thread sonlanıyor

🚀 Client Kullanımı – Hadi Test Edelim

./client push elma   # elma ekle, lesgo!

./client push armut  # armut ekle, tadaaa!

./client pop         # son eklenen pop, lesgo!


Client komutu otomatik encode edilip server’a gidiyor, TRAP ile yanıt alıyorsun.

🗂️ Dosya Yapısı
├── server.c       # Server kodu
├── client.c       # Client kodu
├── Makefile       # Derleme için
├── DATABASE.txt   # Kelimeler burada tutuluyor
└── README.md      # İşte bu yazı, lesgo!

 Derleme ve Çalıştırma
make           # server & client derlendi, tadaaa!
./server       # server açıldı, lesgo!
./client ...   # client ile komut gönder


Server kapanırken port hala doluysa:

pkill -f ./server  # port freed, tadaaa!


Lesgo!  Artık mini REST-like socket API’miz hazır. Client gönderiyor, server decode ediyor, TRAP dönüyor, database güvenli 👍
