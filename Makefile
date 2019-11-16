autotest:
	@inotifywait -m -e attrib _build/bin | \
	while read path action file; \
		do if [[ "$$file" =~ test_.* ]]; then \
			$$path$$file; \
		fi; \
	done
