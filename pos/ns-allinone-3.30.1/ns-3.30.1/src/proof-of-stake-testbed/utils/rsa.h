//https://gist.github.com/andreigasparovici/12b460c1cc53586ed0064edbe9f71e87

#ifndef PROJECT_RSA_H
#define PROJECT_RSA_H

namespace ns3 {
    typedef std::pair<int, int> PublicKey;
    typedef std::pair<int, int> PrivateKey;

    struct Keys {
        PublicKey public_key;
        PrivateKey private_key;
    };

    int log_power(int n, int p, int mod);

    bool rabin_miller(int n);

    int generate_prime();

    int gcd(int a, int b);

    int generate_coprime(int n);

    std::pair<int, int> euclid_extended(int a, int b);

    int modular_inverse(int n, int mod);

    int encrypt(PublicKey key, int value);

    int decrypt(PrivateKey key, int value);

    Keys generate_keys();
}

#endif //PROJECT_RSA_H
