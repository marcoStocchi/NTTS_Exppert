###############################################################################
##
##		Load 'ptable' package and provide some helper functions:
##         - look up cell noise from pTable with true count and cell key
##         - calculate DP epsilon and delta from a given pTable
##
##		Author:	Fabian BACH
##
##		Date:	07 Sep 2020
##
###############################################################################


## load libraries and packages
require(ptable)



## look up cell noise from pTable with true count and cell key
lookup_ptable_noise <- function (pTable, trueCount, cellKey) {
  
#  retval <- .C("lookup_ptable", trueCount, cellKey, cnoise=as.double(0.0))

  #print(retval$cnoise)
  
#  return (retval$cnoise);
  
	i_lookup <- min(trueCount, pTable@pParams@ncat)
	subset(pTable@pTable,
		i == i_lookup &
		p_int_lb <= cellKey &
		p_int_ub >= cellKey
	) -> pLine
	if (nrow(pLine) == 1) {
		noise <- pLine$v
	}
	else {
		cat(paste("ERROR: trueCount =", trueCount, "and cellKey =", cellKey, "returns", nrow(pLine), "lines from pTable!\n"))
		noise <- NULL
	}
	return (noise)
}



## calculate DP epsilon and delta from a given pTable
DP_epsilon_delta_from_ptable <- function (pTable) {
	nMax <- pTable@pParams@ncat
	dMax <- pTable@pParams@D
	jMax <- pTable@pParams@js
	xMax <- nMax + dMax + 1
	xed  <- NULL
#cat(paste("nMax =", nMax, "\n"))
#cat(paste("dMax =", dMax, "\n"))
#cat(paste("xMax =", xMax, "\n"))
#cat(paste("jMax =", jMax, "\n"))
		
	## loop over (independent) true input counts
	for (n in 0:nMax) {
	
		## loop over possible noisy output counts
		for (x in 0:xMax) {
#cat(paste("n =", n, "x =", x, "\n"))
				
				## skip impossible outputs:
				## noise out of bounds or
				if (x < n-dMax | x > n+dMax+1) next
				## non-zero x below small-count threshold
				if (x > 0 & x <= jMax) next
				
				## retrieve Pr[n->x] and Pr [n+1->] from pTable
				pr <- c(0., 0.)
				if (x < xMax)
					pr[1] <- pTable@pMatrix[as.character(n),as.character(x)]
				else
					pr[1] <- 0.
				if (n < nMax)
					pr[2] <- pTable@pMatrix[as.character(n+1),as.character(x)]
				else if (x-1 == 0 | x-1 > jMax)
					pr[2] <- pTable@pMatrix[as.character(n),as.character(x-1)]
				else
					pr[2] <- 0.
#cat(paste("   --> p1 =", pr[1], "p2 =", pr[2], "\n"))
				
				## sort probabilities wlog.
				pr <- sort(pr)
				
				## if smaller pr > 0, calculate eps (del=0)
				if (pr[1] > 0.) {
					eps <- log( pr[2] / pr[1] )
					del <- 0.
				}
				## otherwise, take larger pr as del (eps=0)
				else {
					eps <- 0.
					del <- pr[2]
				}
				
				## store eps/del in new row
				rbind(xed, data.frame(
					"n"   = paste("(", n, ",", n+1, ")", sep=""),
					"x"   = x,
					"eps" = eps,
					"del" = del
				)) -> xed
		}
	}
	
	## print global values to console
	nMaxc <- paste("(", nMax, ",", nMax+1, ")", sep="")
	dig <- 4
	cat (paste("global    : eps = ", round(max(xed$eps),dig), ", del = ", round(max(xed$del),dig), "\n", sep=""))
	cat (paste("n <  ", nMaxc, ": eps = ", round(max(xed[which(xed$n!=nMaxc),]$eps),dig), ", del = ", round(max(xed[which(xed$n!=nMaxc),]$del),dig), "\n", sep=""))
	cat (paste("n >= ", nMaxc, ": eps = ", round(max(xed[which(xed$n==nMaxc),]$eps),dig), ", del = ", round(max(xed[which(xed$n==nMaxc),]$del),dig), "\n", sep=""))
	
	## return the whole data frame
	return (xed)
}



## printing function for (eps,del) data frame
print_eps_del <- function (xed, n, dig=3) {
	xed_n <- xed[which(xed$n==paste("(", n, ",", n+1, ")", sep="")),]
	if (nrow(xed_n)==0) {
		cat (paste("Error: n =", n, "while largest allowed is", xed[nrow(xed),1], "\n"))
		return ()
	}
	print (xed_n)
	cat (paste("max(eps) = ", round(max(xed_n$eps),dig), "\n", sep=""))	
	cat (paste("max(del) = ", round(max(xed_n$del),dig), "\n", sep=""))
}


## microservice stubs...

microservice_on_init <- function(configIn)
{
	print("R Microservice initializing");

	cat(configIn, '\n');

	retval <- "M_INIT";
	
	return(retval);
}


microservice_on_ticket <- function(ticketIn)
{
	# process the ticket and return the 
	# response message that you want the
	# producer to put into the HTTP reply.
	# In case of processing errors, 
	# return an appropriate error message.
	
	cat(ticketIn, '\n');

	# parse input
	# simulate a result
	ticketOut <- ticketIn;

	# return updated ticket
	return (ticketOut);
}

microservice_on_deinit <- function(err)
{
	print("R Microservice deinitializing");

	retval <- "M_DEINIT";

	return(retval);
}
