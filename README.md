
# 🌱 Dendronix - The Root of Your System’s Life 🌱
![dendronix](https://github.com/user-attachments/assets/e05c357e-7666-4677-9ad5-6bae1068e6fa)

### Welcome to Dendronix, the init system that keeps your Linux tree strong and stable!
Forget bloated, overcomplicated init systems—Dendronix is here to boot fast, manage services cleanly, and shut down with grace. 🚀

## 🌟 Why Dendronix?

✔ Light as a leaf – No unnecessary dependencies, just pure C++ efficiency.

✔ Organized like nature – Services are numbered, so they start in order, like a growing tree. 🌳

✔ No zombie processes! – We clean up like a proper gardener. 🧹

✔ Handles shutdown like a pro – SIGTERM? SIGINT? We got it covered.

✔ Simple, readable, and hackable – Want to tweak it? Go ahead!

## 🛠️ Installation

### It’s as simple as planting a seed:
```bash
git clone https://github.com/yourusername/dendronix.git
cd dendronix
make
sudo make install
```

## 🚀 How It Works

### Dendronix looks for service scripts in /etc/dendro/init.d/.
Each script follows this naming pattern:

[number]-[service-name]

For example:

10-network
20-sshd
30-database

### Each script must support start and stop commands:

```bash
#!/bin/sh
case "$1" in
  start) echo "🌱 Growing service...";;
  stop) echo "🍂 Shedding service...";;
  *) echo "Usage: $0 {start|stop}"; exit 1;;
esac
```

### To make Dendronix your system’s root guardian, run:

```bash
sudo /sbin/dendronix
```

## 🔥 Contribute & Join the Forest!

Dendronix is always growing! Contribute, suggest features, or just say hi. 🌿

## 📜 License

Dendronix is released under the GPL License. Use it, modify it, and spread the roots!
