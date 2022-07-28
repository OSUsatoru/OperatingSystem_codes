
## Instruction
Creating five small programs that encrypt and decrypt information using a one-time pad-like system. These programs will combine the multi-processing code you have been learning with socket-based inter-process communication. Your programs will also be accessible from the command line using standard Unix features like input/output redirection, and job control. Finally, you will write a short compilation script. \
Program will encrypt and decrypt plaintext into ciphertext, using a key, in exactly the same fashion as above, except it will be using modulo 27 operations: your 27 characters are the 26 capital letters, and the space character. All 27 characters will be encrypted and decrypted as above.

## Example of run program commands
```sh
./p5testscript RANDOM_PORT1 RANDOM_PORT2 > mytestresults 2>&1
```
## sample output
```sh
$ cat plaintext1
THE RED GOOSE FLIES AT MIDNIGHT STOP
$ enc_server 57171 &
$ dec_server 57172 &
$ keygen 10
EONHQCKQ I
$ keygen 10 > mykey
$ cat mykey
VAONWOYVXP
$ keygen 10 > myshortkey
$ enc_client plaintext1 myshortkey 57171 > ciphertext1
Error: key ‘myshortkey’ is too short
$ echo $?
1
$ keygen 1024 > mykey
$ enc_client plaintext1 mykey 57171 > ciphertext1
$ cat ciphertext1
WANAWTRLFTH RAAQGZSOHCTYS JDBEGYZQDQ
$ keygen 1024 > mykey2
$ dec_client ciphertext1 mykey 57172 > plaintext1_a
$ dec_client ciphertext1 mykey2 57172 > plaintext1_b
$ cat plaintext1_a
THE RED GOOSE FLIES AT MIDNIGHT STOP
$ cat plaintext1_b
WSXFHCJAEISWQRNO L ZAGDIAUAL IGGTKBW
$ cmp plaintext1 plaintext1_a
$ echo $?
0
$ cmp plaintext1 plaintext1_b
plaintext1 plaintext1_b differ: byte 1, line 1
$ echo $?
1
$ enc_client plaintext5 mykey 57171
enc_client error: input contains bad characters
$ echo $?
1
$ enc_client plaintext3 mykey 57172
Error: could not contact enc_server on port 57172
$ echo $?
2
$

```