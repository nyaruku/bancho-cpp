# osu! Bancho Gameserver in c++

## Local Development Setup
Change /etc/hosts to match your dev domain
```
# Some of them might not even be used anymore
127.0.0.1 dev.bancho
127.0.0.1 c1.dev.bancho
127.0.0.1 c2.dev.bancho
127.0.0.1 c3.dev.bancho
127.0.0.1 c4.dev.bancho
127.0.0.1 c5.dev.bancho
127.0.0.1 c6.dev.bancho
```

Install caddy
```
sudo pacman -S caddy

```

Create a Caddyfile where you want
```
dev.bancho *.dev.bancho {
    tls /home/admin/cert/dev.bancho+1.pem /home/admin/cert/dev.bancho+1-key.pem
    reverse_proxy 127.0.0.1:13380
}
```

Now run caddy as root, because it uses ports below 1024
```
sudo caddy run --config ~/cert/Caddyfile
```

Run osu! with --devserver argument
```
osu-wine --devserver dev.bancho
```