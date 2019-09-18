# Environment Setup Guide for Lab Assignment

- Advanced computer security theories and techniques
- [Dept. of Electrical and Computer Engineering](https://ee.snu.ac.kr/en), [Seoul National University](http://snu.ac.kr/index.html)
- Instructor: [Byoungyoung Lee](https://lifeasageek.github.io/) (byoungyoung@snu.ac.kr)

Our lab assignments require you to have an access to a machine running
Ubuntu. This environment setup guides you to install the virtual
machine running Ubuntu.

First, you will need to install
[vagrant](https://www.vagrantup.com/docs/installation/) on your own
machine (check vagrant document to install it on your OS).

Once you installed `vagrant`, then run the following commands.

```sh
# Fetch Ubuntu 14.04 VM from the server.
$ vagrant box add ubuntu/trusty64

# Prepare vagrant settings for the VM in the current working directory.
$ vagrant init ubuntu/trusty64

# Boot-up the VM
$ vagrant up

# ssh into the VM
$ vagrant ssh
```

Now you are in the vm, and run the following commands within vm to
initialize environments.

```sh
[vm]$ cd /vagrant
[vm]:/vagrant$ sudo apt-get update
[vm]:/vagrant$ sudo apt-get install git
[vm]:/vagrant$ git clone -b fall-19 https://github.com/lifeasageek/snu-software-security-public
[vm]:/vagrant$ cd snu-software-security-public/lab-assign-setup
[vm]:/vagrant/snu-software-security-public/lab-assign-setup$ ./init
```

After running the commands above, if your outputs include the
following strings, you are all set!

```
> kflag module installed
F38FE749E36CD0437AD0062D6836C393618FCC4A412FDE666C89EB41AC0814D5
               >>> NOT A VALID FLAG  <<<
2AF0AEC61229004D9E8CF6F4E3622419A37F6D410526951D6EB2F639DBEA49AD
                  
[*] GOOD! init was successful!
```

Note that `snu-software-security-public` folder should be located as
`/vagrant/snu-software-security-public` inside vm. This is required as the kflag
kernel module, which is installed by `init` script and generates
`flag`, assumes the `target` binary (that you will attack in the
assignment) is placed in the directory `/vagrant/snu-software-security-public`.
