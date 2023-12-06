

package utils_pkg
    // determines if value is within [first, last)
    function automatic logic compare_bits
    #(parameter LENGTH)(
        logic[LENGTH-1:0] value,
        logic[LENGTH-1:0] first,
        logic[LENGTH-1:0] last
    );
        compare_bits = (value >= first) && (value < last);
    endfunction

endpackage