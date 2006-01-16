#
#  $Id: regres.pl,v 1.5 2006-01-16 00:56:27 bjoo Exp $
#
#  This is the top-level script used by chroma/scripts/run_chroma_xmldiff.pl
#

sub regresDirs
{
    # 
    # I'd really like to include the regres.pl scripts recursively down 
    # inside the chroma/tests directory, but I'm having difficulty
    # convincing perl to do it. The problem seems to be that the model
    # I want, namely like the c-preprocessor including files that recursively
    # includes other files (in subdirs) is not how the perl "do" works.
    # So, spell out all the many regression dirs and source them individually.
    #
#    return (
#	    "$test_dir/t_leapfrog/regres.pl"
#	    );

     return ( 
	    "$test_dir/chroma/hadron/make_source/regres.pl",
	    "$test_dir/chroma/hadron/propagator/regres.pl",
	    "$test_dir/chroma/hadron/seqsource/regres.pl",
	    "$test_dir/chroma/hadron/spectrum/regres.pl",
	    "$test_dir/chroma/hadron/spectrumOct/regres.pl",
	    "$test_dir/chroma/hadron/mres/regres.pl",
	    "$test_dir/chroma/smear/link_smear/regres.pl",
	    "$test_dir/t_leapfrog/regres.pl",
	    "$test_dir/purgaug/regres.pl"
     );
}

# Return a true value for a require statement
1;
