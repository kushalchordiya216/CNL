import math
import os

switcher = {
    'A': 1,
    'B': 2,
    'C': 3,
    'D': 4
}


def get_class(ip):
    if ip <= '127':
        return "A"
    elif ip >= '128' and ip <= '191':
        return "B"
    elif ip >= "192" and ip <= "223":
        return "C"
    return "D"


def get_mask(sub):
    if sub == "A":
        return "255.0.0.0"
    elif sub == "B":
        return "255.255.0.0"
    elif sub == "C":
        return "255.255.255.0"
    return ""


def get_new_subnet(ip_arr, bit_borrowed, subnet_class):
    temp = ip_arr
    cnt = 0
    for i in range(7, 7-bit_borrowed, -1):
        cnt = cnt + 2**i

    temp[switcher.get(subnet_class)] = str(cnt)
    new_subnet = '.'.join(temp)
    return new_subnet


def check_subnet_ranges(subnet_class, bits_borrowed, num_subnets):
    i = switcher.get(subnet_class)
    division = 2**bits_borrowed
    temp = 256/division

    start = []
    end = []

    for it in range(num_subnets):
        start.append(int(temp*it))
        end.append(int(temp*(it+1) - 1))
    print("subnet ranges are:")
    print(start)
    print(end)


def check_subnet(ip, sub, new_ip):
    ip_arr = ip.split('.')
    new_ip_arr = new_ip.split('.')
    sub_arr = sub.split('.')

    ans = 0

    res1 = []
    res2 = []
    for i in range(4):
        temp1 = int(ip_arr[i]) & int(sub_arr[i])
        temp2 = int(new_ip_arr[i]) & int(sub_arr[i])

        if temp1 != temp2:
            ans = 1

        res1.append(temp1)
        res2.append(temp2)
    print(res1)
    print(res2)
    return ans


if __name__ == "__main__":
    ip = raw_input("Enter ip of this machine:")
    ip_arr = ip.split('.')
    subnet_class = get_class(ip_arr[0])
    mask = get_mask(subnet_class)
    print("subnet class {}, having mask {}".format(subnet_class, mask))
    num_subnets = int(raw_input("Enter number of subnets:"))
    bits_borrowed = int(math.ceil(math.log(num_subnets, 2)))
    new_subnet = get_new_subnet(mask.split('.'), bits_borrowed, subnet_class)
    print("new subnet is ", new_subnet)
    check_subnet_ranges(subnet_class, bits_borrowed, num_subnets)
    new_ip = raw_input("Enter new ip:")
    result = check_subnet(ip, new_subnet, new_ip)
    if result == 0:
        print("Inside same subnet")
    else:
        print("different subnets")

    os.system('sudo ifconfig enp2s0f1 %s netmask %s' % (ip, new_subnet))

    os.system("ifconfig")
    os.system("ping %s" % (new_ip))
