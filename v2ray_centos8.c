#include <stdio.h>
#include <stdlib.h>


FILE* config,* cer;
char uuid[40],sni[30];
int mode;

int main(){
Menu:UI();
    system("clear");
    if (mode == 1) {
        install_xray();
        goto Menu;
    }
    else if (mode == 2) {
        system("systemctl stop xray");
        system("systemctl start xray");
        printf("正在检测xray运行状态，以下输出不为空则运行正常！\n");
        printf("--------------以下输出不为空则xray运行正常------------------\n");
        system("ss -lp | grep xray");
        printf("\n--------------------------------------------------------\n");
        goto Menu;
    }
    else if (mode == 3) {
        printf("xray二维码:\n\n");
        system("qrencode -t ansiutf8 < /usr/local/etc/xray/vmess.txt");
        printf("\n\n");
        printf("Vmess链接:\n\n");
        system("bash /usr/local/etc/xray/code_gen.sh");
        printf("\n--------------------------------------------------------\n");
        printf("server: %s\nport: 2053\nUUID: %s\npath: /iso\n", sni, uuid);
        printf("--------------------------------------------------------\n");
        goto Menu;
    }
    else if (mode == 4) {
        system("vi /usr/local/etc/xray/config.json");
        system("systemctl restart xray");
        printf("正在检测xray运行状态，以下输出不为空则运行正常！\n");
        printf("--------------以下输出不为空则xray运行正常------------------\n");
        system("ss -lp | grep xray");
        printf("\n--------------------------------------------------------\n");
        goto Menu;
    }
    else if (mode == 5) {
        if (fopen("/root/1.pem", "r") == NULL || fopen("/root/2.pem", "r") == NULL) {
            printf("检测到证书与私钥文件未按照规定方式放置于根目录，强制退出！\n");
            exit(0);
        }
        printf("请输入已绑定此服务器ip的新域名:");
        scanf("%s", sni);
        config = fopen("/usr/local/etc/sni.conf", "w");
        fprintf(config, "%s", sni);
        fclose(config);
        config = fopen("/usr/local/etc/xray/uuid.conf", "r");
        fscanf(config, "%s", uuid);
        fclose(config);
        printf("正在复制SSL证书与私钥. . .\n");
        system("cp -rf /root/1.pem /usr/local/etc/xray/certificate.pem");
        system("cp -rf /root/2.pem /usr/local/etc/xray/private.pem");
        system("systemctl restart xray");
        QRCodeGen();
        printf("正在检测xray运行状态，以下输出不为空则运行正常！\n");
        printf("--------------以下输出不为空则xray运行正常------------------\n");
        system("ss -lp | grep xray");
        printf("\n--------------------------------------------------------\n");
        printf("xray部署完成！\n");
        printf("xray二维码:\n\n");
        system("qrencode -t ansiutf8 < /usr/local/etc/xray/vmess.txt");
        printf("\n\n");
        printf("Vmess链接:\n\n");
        system("bash /usr/local/etc/xray/code_gen.sh");
        printf("\n--------------------------------------------------------\n");
        printf("server: %s\nport: 2053\nUUID: %s\npath: /iso\n",sni,uuid);
        printf("--------------------------------------------------------\n");
        goto Menu;
    }
    else if (mode == 6) {
        printf("正在更新xray主程序. . .\n");
        system("wget https://github.com/XTLS/Xray-install/raw/main/install-release.sh -O install-release.sh");
        system("chmod +x install-release.sh && bash install-release.sh");
        system("rm -f install-release.sh");
        system("systemctl start xray");
        printf("xray主程序更新完成！\n");
        printf("正在检测xray运行状态，以下输出不为空则运行正常！\n");
        printf("--------------以下输出不为空则xray运行正常------------------\n");
        system("ss -lp | grep xray");
        printf("\n--------------------------------------------------------\n");
        goto Menu;
    }
    else if (mode == 7) {
        system("systemctl stop xray");
        goto Menu;
    }
    else {
        exit(0);
    }
    return 0;
}

int UI() {
    printf("-----------------------------------------------------------\n");
    printf("----------------------xray安装工具-----------------------\n");
    printf("-----------------------------------------------------------\n");
    printf("安装前或需要更新SSL证书，请将证书(*.cer/*.crt/*.pem)与私钥(*.key/*.pem)分别命名为1.pem与2.pem，上传至服务器/root目录\n");
    printf("-----------------------------------------------------------\n");
    printf("----------------------当前Kernel版本-----------------------\n");
    system("uname -sr");
    printf("-----------------------------------------------------------\n");
    printf("1.安装xray\n2.运行xray\n3.显示配置\n4.修改xray配置\n5.更新域名与SSL证书\n6.更新xray\n7.关闭xray\n0.退出\n");
    printf("-----------------------------------------------------------\n");
    printf("请输入:");
    scanf("%d", &mode);
    return 0;
}

int install_xray() {
    KernelUpdate(); 
    config = fopen("/usr/local/etc/sni.conf", "r");
    fscanf(config, "%s", sni);
    fclose(config);
    system("setenforce 0");
    system("yum install -y curl bind-utils qrencode");
    printf("正在运行xray安装脚本. . .\n");
    system("wget https://github.com/XTLS/Xray-install/raw/main/install-release.sh -O install-release.sh");
    system("chmod +x install-release.sh && bash install-release.sh");
    system("rm -f install-release.sh");
    system("rm -f TCPO.sh");
    printf("正在复制SSL证书与私钥. . .\n");
    system("cp -rf /root/1.pem /usr/local/etc/xray/certificate.pem");
    system("cp -rf /root/2.pem /usr/local/etc/xray/private.pem");
    printf("正在生成配置文件. . .\n");
    system("curl https://raw.githubusercontent.com/HXHGTS/xray-websocket-tls/main/config.json.1 > /usr/local/etc/xray/config.json");
    printf("正在生成UUID. . .\n");
    system("xray uuid > /usr/local/etc/xray/uuid.conf");
    config = fopen("/usr/local/etc/xray/uuid.conf", "r");
    fscanf(config, "%s", uuid);
    fclose(config);
    config = fopen("/usr/local/etc/xray/config.json", "a");
    fprintf(config, "                        \"id\": \"%s\"\n", uuid);
    fclose(config);
    system("curl https://raw.githubusercontent.com/HXHGTS/xray-websocket-tls/main/config.json.2 >> /usr/local/etc/xray/config.json");
    printf("正在启动xray并将xray写入开机引导项. . .\n");
    system("systemctl enable xray");
    system("systemctl start xray");
    QRCodeGen();
    printf("正在检测xray运行状态，以下输出不为空则运行正常！\n");
    printf("--------------以下输出不为空则xray运行正常------------------\n");
    system("ss -lp | grep xray");
    printf("\n--------------------------------------------------------\n");
    printf("xray部署完成！\n");
    printf("xray二维码:\n\n");
    system("qrencode -t ansiutf8 < /usr/local/etc/xray/vmess.txt");
    printf("\n\n");
    printf("Vmess链接:\n\n");
    system("bash /usr/local/etc/xray/code_gen.sh");
    printf("\n--------------------------------------------------------\n");
    printf("server: %s\nport: 2053\nUUID: %s\npath: /iso\n", sni, uuid);
    printf("--------------------------------------------------------\n");
    return 0;
}

int QRCodeGen() {
    config = fopen("/usr/local/etc/xray/code_gen.sh", "w");
    fprintf(config, "#!/bin/bash\n");
    fprintf(config, "VMESSCODE=$(base64 -w 0 << EOF\n");
    fprintf(config, "    {\n");
    fprintf(config, "      \"v\": \"2\",\n");
    fprintf(config, "      \"ps\": \"v2ray\",\n");
    fprintf(config, "      \"add\": \"%s\",\n",sni);//Addr
    fprintf(config, "      \"port\": \"2053\",\n");
    fprintf(config, "      \"id\": \"%s\",\n",uuid);//UUID
    fprintf(config, "      \"aid\": \"0\",\n");
    fprintf(config, "      \"net\": \"ws\",\n");
    fprintf(config, "      \"type\": \"none\",\n");
    fprintf(config, "      \"host\": \"%s\",\n",sni);
    fprintf(config, "      \"path\": \"/iso\",\n");
    fprintf(config, "      \"tls\": \"tls\"\n");
    fprintf(config, "    }\n");
    fprintf(config, "EOF\n");
    fprintf(config, "    )\n");
    fprintf(config, "echo vmess://${VMESSCODE}\n");
    fclose(config);
    system("bash /usr/local/etc/xray/code_gen.sh > /usr/local/etc/xray/vmess.txt");
    return 0;
}

int KernelUpdate() {
    if ((fopen("KernelUpdate.sh", "r")) == NULL) {
        if (fopen("/root/1.pem", "r") == NULL || fopen("/root/2.pem", "r") == NULL) {
        printf("检测到证书与私钥文件未按照规定方式放置于根目录，强制退出！\n");
        exit(0);
    }
        printf("请输入已绑定此服务器ip的域名:");
        scanf("%s", sni);
        config = fopen("/usr/local/etc/sni.conf", "w");
        fprintf(config, "%s", sni);
        fclose(config);
        printf("正在升级新内核. . .\n");
        system("wget https://cdn.jsdelivr.net/gh/HXHGTS/TCPOptimization/KernelUpdate8.sh -O KernelUpdate.sh");
        system("chmod +x KernelUpdate.sh");
        printf("正在升级，将自动触发重启以应用配置. . .\n");
        system("bash KernelUpdate.sh");
    }
    else {
        system("curl -sSL https://cdn.jsdelivr.net/gh/HXHGTS/TCPOptimization/TCPO_centos8.sh | sh");
    }
    return 0;
}
