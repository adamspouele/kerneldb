({
	"name": "Executor",
	"author": "subprotocol",
	"description": "Stores values and allows functions to be dynamically pushed into the data.",
	"merge": function(key, old, value) {
		if (typeof value == "string") {
			eval("var value = " + value);
			
			if (value instanceof Function)
				return value(old);
		}

		return value;
	}
});