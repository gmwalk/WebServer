set exrc
set secure
set tabstop=4
set softtabstop=4
set shiftwidth=4
set noexpandtab
set colorcolumn=110
highlight ColorColumn ctermbg=darkgray
augroup project
	automd!
	autocmd BufRead, BufNewFile *.h, *.c filetype=c.docygen
augroup END
