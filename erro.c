

void InodeReleaseTofreeCotlab(void* n)
{
	if (!n) return;
	inode* nod = n;
	memf(nod->addr);
	CotResourceRemove(nod->data, nod->type);
	memf(nod);
}
