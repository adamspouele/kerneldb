({
	"name": "Counter",
	"author": "subprotocol",
	"description": "Increments old value by new value.",
	"merge": function(key, old, value, log) {
		return (old||0) + value;
	},
	"partialMerge": function(key, left, right, log) {
		return left + right;
	}
});