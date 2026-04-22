FROM gcr.io/oss-fuzz-base/base-builder

RUN set -eux; \
    sed -i 's|http://archive.ubuntu.com/ubuntu|https://archive.ubuntu.com/ubuntu|g; s|http://security.ubuntu.com/ubuntu|https://security.ubuntu.com/ubuntu|g' /etc/apt/sources.list; \
    apt-get -o Acquire::Retries=5 update; \
    apt-get install -y --no-install-recommends \
        make \
        gcc \
        python3-clang; \
    rm -rf /var/lib/apt/lists/*

# Copy PSM source files
COPY src/ $SRC/psm-project/src/
COPY include/ $SRC/psm-project/include/
COPY stub/ $SRC/psm-project/stub/

COPY fuzzer.c $SRC/psm-project/fuzzer.c

WORKDIR $SRC/psm-project
COPY build.sh $SRC/
