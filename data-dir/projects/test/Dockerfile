FROM gcr.io/oss-fuzz-base/base-builder

# Switch to HTTPS mirrors to avoid 504 timeouts through corporate proxy
RUN set -eux; \
    sed -i 's|http://archive.ubuntu.com/ubuntu|https://archive.ubuntu.com/ubuntu|g; s|http://security.ubuntu.com/ubuntu|https://security.ubuntu.com/ubuntu|g' /etc/apt/sources.list; \
    apt-get -o Acquire::Retries=5 update; \
    apt-get install -y --no-install-recommends \
        python3-clang; \
    rm -rf /var/lib/apt/lists/*

# Copy test project source files
COPY src/     $SRC/test-project/src/
COPY include/ $SRC/test-project/include/

WORKDIR $SRC/test-project
COPY build.sh $SRC/
