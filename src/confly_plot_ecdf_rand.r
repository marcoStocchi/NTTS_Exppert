# Benchmarking draws from Laplace
# and Geometric distributions 
# performed using Confly routines

# n (unsigned int): size of test
#
# params: triplet of floating point
#   noise params for laplace and 
#   geometric distributions:
#   - mu (floating p.): laplace 
#        location 
#   - b (floating p.): laplace 
#        scaling 
#   - eps (floating p.): geometric 
#        epsilon

confly_plot_ecdf_rand <- function(
    topPath, 
    inFile, outFile,
    n, params)
{
    # compose absolute path
    # to input and output files
    abspathIn <- paste(
        topPath, "/", 
        inFile, sep="")
    abspathOut<- paste(
        topPath, "/", 
        outFile, sep="")

    # read csv file
    valrand <- read.csv(
        abspathIn, 
        sep=";", 
        header=FALSE)

    # set column names
    colnames(valrand) <- c(
        "LAP", "GEO")

    # find variances
    varlap <- var(valrand[, "LAP"])
    vargeo <- var(valrand[, "GEO"])

    # prep image file
    png(file=abspathOut, 
        width=600, 
        height=450)

    # import noise params
    mu <- params[[1]][1]
    b <- params[[2]][1]
    eps <- params[[3]][1]

    # prep plot title
    mtext <- paste(
        "benchmarking ", 
        n, 
        " draws from Laplace and",
        " Geometric distributions",
        "\n( ",
        "Laplace ", 
            "mu=", mu, ", ",
            "b=", b, ", ",
        "Geometric ",
            "eps=", eps, 
        ")\nengine: Confly")

    # do plots...
    plot.ecdf(valrand[, "LAP"], 
        col="black", 
        xlab="x", ylab="P", 
            cex=0.8, pch=1,
                main=mtext)

    lines(ecdf(valrand[, "GEO"]), 
        col="red", lwd=0,
            cex=0.8, pch=2)

    # text coords
    xtext <- 8
    ytext <- 0.7

    # do texts...
    text(xtext, ytext, 
        expression(
        sigma^2 == paste(
            frac(1,N), " ", 
                sum((x[i]-mu)^2, 
                    i==1, N))))

    text(xtext, ytext-.2, 
        bquote(sigma^2 
        == .(varlap)))

    text(xtext, ytext-.4, 
        bquote(sigma^2
        == .(vargeo)), col='red')

    # legend
    legend('bottomright', 
        legend=c("Laplace",
            "Geometric"),
        col=c("black","red"),
        pch=c(0, 2))

    dev.off()
}

