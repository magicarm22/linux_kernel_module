name_client = client
name_daemon = daemon
name_kernel = kernel
name_lib = libmylib.a

client_files = main.c
daemon_files = main.c

lib_files = ft_itoa.c

client_folder = ./client/
daemon_folder = ./daemon/
lib_folder = ./Lib/

libs = -L$(lib_folder) -lmylib 
includes = -I$(lib_folder)

src_client = $(addprefix $(client_folder), $(client_files))
src_daemon = $(addprefix $(daemon_folder), $(daemon_files))
src_lib = $(addprefix $(lib_folder), $(lib_files))

obj_files_client = $(patsubst %.c, %.o, $(client_files))
obj_files_daemon = $(patsubst %.c, %.o, $(daemon_files))
obj_files_lib = $(patsubst %.c, %.o, $(lib_files))

obj_client = $(addprefix $(client_folder), $(obj_files_client))
obj_daemon = $(addprefix $(daemon_folder), $(obj_files_daemon))
obj_lib = $(addprefix $(lib_folder), $(obj_files_lib))


kernel_path = kernel
kernel_file = my_kernel
kernel_full = $(kernel_path)/$(kernel_file)

device_name = /dev/my_kernel

obj-m += kernel/my_kernel.o

all: $(name_lib) $(name_client) $(name_daemon)
	@make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules 

$(name_client): $(obj_client)
	gcc  $(libs) $(includes) $(obj_client) -o $(client_folder)$(name_client)

$(name_daemon): $(obj_daemon)
	gcc $(libs) $(includes) $(obj_daemon) -o $(daemon_folder)$(name_daemon)

$(name_lib): $(obj_lib)
	gcc -c $(src_lib)
	ar rc $(lib_folder)$(name_lib) $(obj_lib)

$(client_folder)%.o : $(client_folder)%.c
	gcc -c $(includes) $< -o $@

$(daemon_folder)%.o : $(daemon_folder)%.c
	gcc -c $(includes) $< -o $@

$(lib_folder)%.o : $(lib_folder)%.c
	gcc -c $(includes) $< -o $@

clean:
	@make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	@rm -rf client/client
	@rm -rf daemon/daemon

insert_kernel:
	@-sudo rmmod $(kernel_file)
	@sudo dmesg -C
	@sudo insmod $(kernel_full).ko

re: clean all

device: insert_kernel
	@dmesg

test: insert_kernel
	tail -n 20 $(device_name)
