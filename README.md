#temp
sudo caddy run --config ~/cert/Caddyfile
dev.bancho *.dev.bancho {
    tls /home/admin/cert/dev.bancho+1.pem /home/admin/cert/dev.bancho+1-key.pem
    reverse_proxy 127.0.0.1:13380
}

osu-wine --devserver dev.bancho
