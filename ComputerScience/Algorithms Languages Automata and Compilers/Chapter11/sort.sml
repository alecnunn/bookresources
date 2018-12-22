fun listmin (e::nil) = e
|   listmin (h::tail) = if h < listmin tail then h else listmin tail

fun append (e, []) = (e::nil)
|   append (e, h::tail) = (h::append(e, tail))

fun rotate [] = []
|   rotate (h::tail) = if h = listmin (h::tail) then (h::tail)
                       else rotate (append(h, tail))

fun sort [] = []
|   sort (h::tail) = 
    let val (nh::ntail) = rotate (h::tail) in
        (nh::sort(ntail)) 
    end
