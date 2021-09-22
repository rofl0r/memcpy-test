#!/usr/bin/env python

import sys, os

candidates = []
order = []
tmp = "/tmp/memcpy.tmp."
fna = ['']
for i in range(1, len(sys.argv)):
	fn = sys.argv[i]
	cached = True if fn.startswith('c:') else False
	if cached: fn = fn[2:]
	fna.append(fn)
	if cached and os.path.exists("%s.%s"%(tmp, fn)): pass
	elif os.system("./build.sh %s && ./test > %s.%s"%(fn, tmp, fn)):
		sys.exit(1)
	results = {}
	with open("%s.%s"%(tmp, fn), "r") as f:
		a = f.read().split('\n')
		for x in a:
			if not '\t' in x: continue
			n, r = x.split('\t')
			results[n] = r
			if i == 1: order.append(n)
	candidates.append(results)

def max(a, b):
	return a if a > b else b

col_lengths = [len(x) for x in sys.argv]
col_lengths[0] = 4
for n in order: col_lengths[0] = max(len(n), col_lengths[0])
last_n = order[len(order)-1]
for i in range(len(candidates)):
	col_lengths[i+1] = max(len(candidates[i][last_n]), col_lengths[i+1])

def fmt(s, col, dir=-1):
	if dir == -1:
		while len(s) < col_lengths[col]: s = ' ' + s
	else:
		while len(s) < col_lengths[col]: s = s + ' '
	return s

print ("memcpy speed comparison")
print ("CC=%s, OPTS=%s, UNIT=ticks"%( \
	os.environ['CC'] if 'CC' in os.environ else 'cc', \
	os.environ['OPTS'] if 'OPTS' in os.environ else ''))
s = fmt("size", 0, 1)
for i in range(1, len(fna)):
	s += " " + fmt(fna[i], i, 1)
print (s)
print ('-'*80)

for n in order:
	s = '%s '%fmt(n, 0)
	for i in range(len(candidates)): s += fmt(candidates[i][n], i+1) + ' '
	print(s)
