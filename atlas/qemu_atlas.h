#ifndef QEMU_ATLAS_H
#define QEMU_ATLAS_H

#ifdef __cplusplus
extern "C" {
#endif

	int main_qemu(int argc, char **argv, char **envp);
	int atlas_main(int argc,char** argv);

	void qemu_atlas_init(void);

#ifdef __cplusplus
}
#endif

#endif /* QEMU_ATLAS_H */
