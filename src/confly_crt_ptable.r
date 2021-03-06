## create a ptable and write it to file
## this R script will be called by libconfly
## in order to use the ptable package of SDCTools

source("ptable.r")

## BN (CK) parameters: 
## bound D_CK, 
## variance V_CK, 
## small-count threshold js_CK
# D_CK <- 3
# V_CK <- 1.5
# js_CK <- 1

confly_crt_ptable <- function(fname, D_CK, V_CK, js_CK)
{
  ptab <- create_cnt_ptable (D_CK, V_CK, js_CK)
  
  #print(ptab@pTable[, c("i", "j", "p", "v", "p_int_lb", "p_int_ub")])
  
  write.table(ptab@pTable[, 
    c("i", "j", "p", "v", "p_int_lb", "p_int_ub")], 
    file = fname, 
    row.names = FALSE, 
    col.names = FALSE)
}

