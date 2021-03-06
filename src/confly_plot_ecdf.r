## plot the ECDF using the traversing response
## file produced by the Confly tool


#topPath <- "/home/eshome/stocmar/DP/data/output/"

confly_plot_ecdf <- function(topPath, inFile, outFile)
{
    # cat(topPath, "\n")
    # cat(inFile, "\n")
    # cat(outFile, "\n")

    t1<-strsplit(inFile, ".", fixed = TRUE)

    params<-strsplit(t1[[1]][1], "_")

    geo=params[[1]][2]
    sex=params[[1]][3]
    age=params[[1]][4]
    yae=params[[1]][5]

    #cat(geo, sex, age, yae, "\n", sep=" ")

    # compose absolute path
    # to input and output files
    abspathIn <- paste(topPath, "/", inFile, sep="")
    abspathOut<- paste(topPath, "/", outFile, sep="")
    
    # read traversing file
    trav <- read.csv(abspathIn, sep=";", header=FALSE)

    # set column names
    colnames(trav) <- c(
        "count", 
        "nCK", "nDP_f", "nDP_g",
        "CK", "DP_f", "DP_g")

    #cat (trav[, "count"], "\n")

    # find variances
    varcnt = var(trav[, "count"])
    varCK = var(trav[, "nCK"])
    varDPF = var(trav[, "nDP_f"])
    varDPG = var(trav[, "nDP_g"])
    # cat("nonoise:", varcnt, "\n")
    # cat("cellkey:", varCK, "\n")
    # cat("laplace:", varDPF, "\n")
    # cat("geometr:", varDPG, "\n")

    # find min
    mincount<-min(trav[, "count"])
    minCK<-min(trav[, "CK"])
    minDPF<-min(trav[, "DP_f"])
    minDPG<-min(trav[, "DP_g"])

    absmin<- min(mincount, minCK, minDPF, minDPG)

    # print(absmin)  

    # find max
    maxcount<-max(trav[, "count"])
    maxCK<-max(trav[, "CK"])
    maxDPF<-max(trav[, "DP_f"])
    maxDPG<-max(trav[, "DP_g"])

    absmax<- max(maxcount, maxCK, maxDPF, maxDPG)

    # print(absmax)  

    domain<-(absmin:absmax)

    # print (mincount)
    # print (minCK)
    # print (minDPF)
    # print (minDPG)
    # 
    # print (maxcount)
    # print (maxCK)
    # print (maxDPF)
    # print (maxDPG)

    #print (domain)

    png(file=abspathOut, width=600, height=450)

    op <- par(cex = 1.2)

    plot.ecdf(domain, cex=0.0, lwd=0,
            #ylim=c(-1.0, 1.2),
            xlab="count", ylab="P", main="")
    lines(ecdf(trav[, "count"]), col="black", cex=1.0, lwd=1.8, pch=3, verticals=TRUE)
    lines(ecdf(trav[,"CK"]), col="darkgreen", cex=1.0, lwd=1, pch=1, verticals=TRUE)
    lines(ecdf(trav[,"DP_f"]), col="red", cex=0.9, lwd=1, pch=6, verticals=TRUE)
    lines(ecdf(trav[,"DP_g"]), col="blue", cex=0.9, lwd=1, pch=5, verticals=TRUE)

    legend('bottomright', 
            legend=c("No noise","Cell Key",
                    "DP-f",
                    "DP-g"),  # text in the legend
            col=c("black","darkgreen",
                "red",
                "blue"),  # point colors
            pch=c(3, 1, 6, 5), bty="y")

    dev.off()
}
